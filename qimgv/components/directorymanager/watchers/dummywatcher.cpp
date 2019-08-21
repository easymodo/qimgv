#include "dummywatcher.h"
#include "directorywatcher_p.h"
#include <QDebug>

#define TAG         "[DummyWatcher]"
#define MESSAGE     "Directory watcher isn't yet implemented for your operating system"

class DummyWatcherWorker : public WatcherWorker {
  public:
    DummyWatcherWorker() {}
    virtual void run() override {
        qDebug() << TAG << MESSAGE;
    }
};

class DummyWatcherPrivate : public DirectoryWatcherPrivate {
  public:
    DummyWatcherPrivate(DirectoryWatcher* watcher) : DirectoryWatcherPrivate(watcher, new DummyWatcherWorker()) {}
};

DummyWatcher::DummyWatcher() : DirectoryWatcher(new DummyWatcherPrivate(this))
{
    qDebug() << TAG << MESSAGE;
}
