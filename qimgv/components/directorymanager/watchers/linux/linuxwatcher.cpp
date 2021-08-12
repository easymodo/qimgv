#include <QTimer>

#include <sys/inotify.h>

#include "linuxwatcher_p.h"
#include "linuxworker.h"

#define TAG                 "[LinuxDirectoryWatcher]"
#define INOTIFY_EVENT_MASK  IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE

/**
  * Time to wait for rename event. If event take time longer
  * than specified then event will be considered as remove event
  */

// TODO: this may break event order.
// Implement a proper queue.
#define EVENT_MOVE_TIMEOUT      150 // ms
#define EVENT_MODIFY_TIMEOUT    150 // ms

LinuxWatcherPrivate::LinuxWatcherPrivate(LinuxWatcher* qq) :
    DirectoryWatcherPrivate(qq, new LinuxWorker()),
    watcher(-1),
    watchObject(-1)
{
    watcher = inotify_init();
}

int LinuxWatcherPrivate::indexOfWatcherEvent(uint cookie) const {
    for (int i = 0; i < watcherEvents.size(); ++i) {
        auto event = watcherEvents.at(i);
        if (event->cookie() == cookie) {
            return i;
        }
    }
    return -1;
}

int LinuxWatcherPrivate::indexOfWatcherEvent(const QString& name) const {
    for (int i = 0; i < watcherEvents.size(); ++i) {
        auto event = watcherEvents.at(i);
        if (event->name() == name) {
            return i;
        }
    }
    return -1;
}

void LinuxWatcherPrivate::dispatchFilesystemEvent(LinuxFsEvent* e) {
    Q_Q(LinuxWatcher);

    uint dataOffset = 0;
    QScopedPointer<LinuxFsEvent> event(e);

    while (dataOffset < event->dataSize()) {
        inotify_event* notify_event = (inotify_event*) (event->data() + dataOffset);
        dataOffset += sizeof(inotify_event) + notify_event->len;

        int mask        = notify_event->mask;
        QString name    = notify_event->name;
        uint cookie     = notify_event->cookie;
        bool isDirEvent = mask & IN_ISDIR;
        
        // Skip events for directories and files that isn't in filter range
        /*if((isDirEvent) && !(mask & IN_MOVED_TO) ) {
            continue;
        }*/

        if (mask & IN_MODIFY) {
            handleModifyEvent(name);
        } else if (mask & IN_CREATE) {
            handleCreateEvent(name);
        } else if (mask & IN_DELETE) {
            handleDeleteEvent(name);
        } else if (mask & IN_MOVED_FROM) {
            handleMovedFromEvent(name, cookie);
        } else if (mask & IN_MOVED_TO) {
            handleMovedToEvent(name, cookie);
        }
    }
}

void LinuxWatcherPrivate::handleModifyEvent(const QString &name) {
    // Find the same event in the list by file name
    int eventIndex = indexOfWatcherEvent(name);
    if (eventIndex == -1) {
        // This is this first modify event for the current file
        int timerId = startTimer(EVENT_MODIFY_TIMEOUT);
        auto event = new WatcherEvent(name, timerId, WatcherEvent::Modify);
        watcherEvents.append(QSharedPointer<WatcherEvent>(event));
    } else {
        auto event = watcherEvents.at(eventIndex);
        // Restart timer again
        killTimer(event->timerId());
        int timerId = startTimer(EVENT_MODIFY_TIMEOUT);
        event->setTimerId(timerId);
    }
}

void LinuxWatcherPrivate::handleDeleteEvent(const QString &name) {
    Q_Q(LinuxWatcher);
    emit q->fileDeleted(name);
}

void LinuxWatcherPrivate::handleCreateEvent(const QString &name) {
    Q_Q(LinuxWatcher);
    emit q->fileCreated(name);
}

void LinuxWatcherPrivate::handleMovedFromEvent(const QString &name, uint cookie) {
    int timerId = startTimer(EVENT_MOVE_TIMEOUT);
    // Save timer id to find out later which event timer is running
    auto event = new WatcherEvent(name, cookie, timerId, WatcherEvent::MovedFrom);
    watcherEvents.append(QSharedPointer<WatcherEvent>(event));
}

void LinuxWatcherPrivate::handleMovedToEvent(const QString &name, uint cookie) {
    Q_Q(LinuxWatcher);

    // Check if file waiting to be renamed
    int eventIndex = indexOfWatcherEvent(cookie);
    if (eventIndex == -1) {
        // No one event waiting for rename so this is a new file
        emit q->fileCreated(name);
    } else {
        // Waiting for rename event is found
        auto watcherEvent = watcherEvents.takeAt(eventIndex);
        // Kill associated timer
        killTimer(watcherEvent->timerId());
        emit q->fileRenamed(watcherEvent->name(), name);
    }
}

void LinuxWatcherPrivate::timerEvent(QTimerEvent *timerEvent) {
    Q_Q(LinuxWatcher);

    // Loop through waiting move events
    int lastIndex = watcherEvents.size() - 1;
    for (int i = lastIndex; i >= 0; --i) {
        auto watcherEvent = watcherEvents.at(i);

        if (watcherEvent->timerId() == timerEvent->timerId()) {
            int type = watcherEvent->type();
            if (type == WatcherEvent::MovedFrom) {
                // Rename event didn't happen so treat this event as remove event
                emit q->fileDeleted(watcherEvent->name());
            } else if (type == WatcherEvent::Modify) {
                emit q->fileModified(watcherEvent->name());
            }

            watcherEvents.removeAt(i);
            break;
        }
    }

    // Stop timer anyway
    killTimer(timerEvent->timerId());
}

LinuxWatcher::LinuxWatcher() : DirectoryWatcher(new LinuxWatcherPrivate(this)) {
    Q_D(LinuxWatcher);

    connect(d->workerThread.data(), &QThread::started, d->worker.data(), &WatcherWorker::run);
    d->worker.data()->moveToThread(d->workerThread.data());

    auto linuxWorker = static_cast<LinuxWorker*>(d->worker.data());
    linuxWorker->setDescriptor(d->watcher);

    connect(linuxWorker, &LinuxWorker::fileEvent,
            d, &LinuxWatcherPrivate::dispatchFilesystemEvent);

    // Here's no need to destroy thread and worker. They're will be removed automatically
    connect(linuxWorker, &LinuxWorker::finished, d->workerThread.data(), &QThread::quit);

    connect(linuxWorker, &LinuxWorker::started, this, &LinuxWatcher::observingStarted);
    connect(linuxWorker, &LinuxWorker::finished, this, &LinuxWatcher::observingStopped);
}

LinuxWatcher::~LinuxWatcher() {
    Q_D(LinuxWatcher);
    int removeStatusCode = inotify_rm_watch(d->watcher, d->watchObject);
    if (removeStatusCode != 0) {
        qDebug() << TAG << "Cannot remove inotify watcher instance:" << strerror(errno);
    }
}

void LinuxWatcher::setWatchPath(const QString& path) {
    Q_D(LinuxWatcher);
    DirectoryWatcher::setWatchPath(path);

    // Subscribe for specified filesystem events
    if (d->watchObject != -1) {
        int status = inotify_rm_watch(d->watcher, d->watchObject);
        if (status == -1) {
            qDebug() << TAG << "Error:" << strerror(errno);
        }
    }

    // Add new path to be watched by inotify
    d->watchObject = inotify_add_watch(d->watcher, path.toStdString().data(), INOTIFY_EVENT_MASK);
    if (d->watchObject == -1) {
        qDebug() << TAG << "Error:" << strerror(errno);
    }
}
