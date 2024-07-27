#include "directorywatcher_p.h"

#if defined(__linux__) || defined(__FreeBSD__)
#include "linux/linuxwatcher.h"
#elif _WIN32
#include "windows/windowswatcher.h"
#elif __unix__
// TODO: implement this
#include "dummywatcher.h"
#elif __APPLE__
// TODO: implement this
#include "dummywatcher.h"
#else
// TODO: implement this
#include "dummywatcher.h"
#endif

#define TAG         "[DirectoryWatcher]"

DirectoryWatcherPrivate::DirectoryWatcherPrivate(DirectoryWatcher* qq, WatcherWorker* w) :
    q_ptr(qq),
    worker(w),
    workerThread(new QThread())
{
}

DirectoryWatcher::~DirectoryWatcher() {
    delete d_ptr;
    d_ptr = nullptr;
}

// Move this function to some creational class
DirectoryWatcher *DirectoryWatcher::newInstance()
{
    DirectoryWatcher* watcher;

#if defined(__linux__) || defined(__FreeBSD__)
        watcher = new LinuxWatcher();
#elif _WIN32
        watcher = new WindowsWatcher();
#elif __unix__
        watcher = new DummyWatcher();
#elif __APPLE__
        watcher = new DummyWatcher();
#else
        watcher = new DummyWatcher();
#endif

    return watcher;
}

void DirectoryWatcher::setWatchPath(const QString& path) {
    Q_D(DirectoryWatcher);
    d->currentDirectory = path;
}

QString DirectoryWatcher::watchPath() const {
    Q_D(const DirectoryWatcher);
    return d->currentDirectory;
}

void DirectoryWatcher::observe()
{
    Q_D(DirectoryWatcher);
    if(!isObserving()) {
        // Reuse worker instance
        d->worker->setRunning(true);
        d->workerThread->start();
    }
    //qDebug() << TAG << "Observing path:" << d->currentDirectory;
}

void DirectoryWatcher::stopObserving()
{
    Q_D(DirectoryWatcher);
    d->worker->setRunning(false);
}

bool DirectoryWatcher::isObserving()
{
    Q_D(DirectoryWatcher);
    return d->workerThread->isRunning();
}

DirectoryWatcher::DirectoryWatcher(DirectoryWatcherPrivate* ptr) {
    d_ptr = ptr;
}
