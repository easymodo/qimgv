#include "watcherworker.h"
#include <QDebug>

WatcherWorker::WatcherWorker()
{
}

void WatcherWorker::setRunning(bool running) {
    isRunning.fetchAndStoreRelaxed(running);
}
