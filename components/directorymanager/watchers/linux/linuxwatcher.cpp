#include <QTimer>

#include <sys/inotify.h>

#include "../private/linuxdirectorywatcher_p.h"
#include "linuxworker.h"

#define TAG                 "[LinuxDirectoryWatcher]"
#define INOTIFY_EVENT_MASK  IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE

/**
  * Time to wait for rename event. If event take time longer
  * than specified then event will be considered as remove event
  */
#define EVENT_MOVE_TIMEOUT 500 // ms

LinuxWatcherPrivate::LinuxWatcherPrivate(LinuxWatcher* qq) :
    DirectoryWatcherPrivate(qq, new LinuxWorker()),
    watcher(-1),
    watchObject(-1)
{
    watcher = inotify_init();
}

int LinuxWatcherPrivate::indexOfMoveEvent(uint cookie) const {
    for (int i = 0; i < moveEvents.size(); ++i) {
        auto event = moveEvents.at(i);
        if (event->cookie() == cookie) {
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
        if (isDirEvent || !isFileNeeded(name)) {
            continue;
        }

        if (mask & IN_MODIFY) {
            emit q->fileModified(name);
        } else if (mask & IN_CREATE) {
            emit q->fileCreated(name);
        } else if (mask & IN_DELETE) {
            emit q->fileDeleted(name);
        } else if (mask & IN_MOVED_FROM) {
            int timerId = startTimer(EVENT_MOVE_TIMEOUT);
            // Save timer id to find out later which event timer is running
            auto delayedEvent = new WatcherEvent(name, cookie, timerId, WatcherEvent::MovedFrom);
            moveEvents.append(QSharedPointer<WatcherEvent>(delayedEvent));
        } else if (mask & IN_MOVED_TO) {
            // Check if file waiting to be renamed
            int eventIndex = indexOfMoveEvent(cookie);
            if (eventIndex == -1) {
                // No one event waiting for rename so this is a new file
                emit q->fileCreated(name);
            } else {
                // Waiting for rename event is found
                auto watcherEvent = moveEvents.takeAt(eventIndex);
                // Kill associated timer
                killTimer(watcherEvent->timerId());
                emit q->fileRenamed(watcherEvent->name(), name);
            }
        }
    }
}

void LinuxWatcherPrivate::timerEvent(QTimerEvent *timerEvent) {
    Q_Q(LinuxWatcher);

    bool eventAbsorbed = false;

    // Loop through waiting move events
    for (int i = 0; i < moveEvents.size(); ++i) {
        QSharedPointer<WatcherEvent> watcherEvent = moveEvents.at(i);
        if (watcherEvent->timerId() == timerEvent->timerId()) {
            // Rename event didn't happen so treat this event as remove event
            emit q->fileDeleted(watcherEvent->name());
            // Get rid of this event
            moveEvents.removeAt(i);
            eventAbsorbed = true;
            break;
        }
    }

    if (!eventAbsorbed) {
        qDebug() << TAG << "Event happened but no one wants it :(";
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

LinuxWatcher::~LinuxWatcher()
{
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
