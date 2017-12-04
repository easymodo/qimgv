#include "directorywatcher_p.h"

#include <QRegExp>

#ifdef __linux__
#include "linux/linuxwatcher.h"
#elif _WIN32
#include "dummywatcher.h"
//#include "windows/windowsdirectorywatcher.h"
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

bool DirectoryWatcherPrivate::isFileNeeded(const QString &fileName) const {
    QStringList filters = currentDirectory.nameFilters();
    foreach (const QString& filter, filters) {
        QRegExp re(filter);
        re.setPatternSyntax(QRegExp::Wildcard);
        if (re.exactMatch(fileName)) {
            return true;
        }
    }
    return false;
}

DirectoryWatcher::~DirectoryWatcher() {
    delete d_ptr;
    d_ptr = 0;
}

// Move this function to some creational class
DirectoryWatcher *DirectoryWatcher::newInstance()
{
    DirectoryWatcher* watcher;

#ifdef __linux__
        watcher = new LinuxWatcher();
#elif _WIN32
        //watcher = new WindowsDirectoryWatcher();
        watcher = new DummyWatcher();
#elif __unix__
        watcher = new DummyWatcher();
#elif __APPLE__
        watcher = new DummyWatcher();
#else
        watcher = new DummyWatcher();
#endif

    return watcher;
}

QStringList DirectoryWatcher::fileFilters() const
{
    Q_D(const DirectoryWatcher);
    return d->currentDirectory.nameFilters();
}

void DirectoryWatcher::setFileFilters(const QStringList& filters) {
    Q_D(DirectoryWatcher);
    d->currentDirectory.setNameFilters(filters);
}

void DirectoryWatcher::setWatchPath(const QString& path) {
    Q_D(DirectoryWatcher);
    d->currentDirectory.setPath(path);
}

QString DirectoryWatcher::watchPath() const {
    Q_D(const DirectoryWatcher);
    return d->currentDirectory.absolutePath();
}

void DirectoryWatcher::observe()
{
    Q_D(DirectoryWatcher);
    if (!d->workerThread->isRunning()) {
        // Reuse worker instance
        d->worker->setRunning(true);
        d->workerThread->start();
    }
    qDebug() << TAG << "Observing path:" << d->currentDirectory.absolutePath();
}

void DirectoryWatcher::stopObserving()
{
    Q_D(DirectoryWatcher);
    d->worker->setRunning(false);
}

QStringList DirectoryWatcher::fileNames() const {
    Q_D(const DirectoryWatcher);
    return d->currentDirectory.entryList(QDir::Files);
}

DirectoryWatcher::DirectoryWatcher(DirectoryWatcherPrivate* ptr) {
    d_ptr = ptr;
}
