#include "loader.h"

Loader::Loader() {
    pool = new QThreadPool(this);
    pool->setMaxThreadCount(2);
}

void Loader::clearTasks() {
    clearPool();
    pool->waitForDone();
}

void Loader::loadBlocking(QString path) {
    if(bufferedTasks.contains(path)) {
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(path);
    runnable->setAutoDelete(false);
    tasks.insert(path, runnable);
    connect(runnable, SIGNAL(finished(std::shared_ptr<Image>, QString)),
            this, SLOT(onLoadFinished(std::shared_ptr<Image>, QString)), Qt::UniqueConnection);
    runnable->run();
}

// clears all buffered tasks before loading
void Loader::loadExclusive(QString path) {
    clearPool();
    load(path, 1);
}

void Loader::load(QString path) {
    load(path, 0);
}

void Loader::load(QString path, int priority) {
    if(tasks.contains(path)) {
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(path);
    runnable->setAutoDelete(false);
    tasks.insert(path, runnable);

    connect(runnable, SIGNAL(finished(std::shared_ptr<Image>, QString)),
            this, SLOT(onLoadFinished(std::shared_ptr<Image>, QString)), Qt::UniqueConnection);
    pool->start(runnable, priority);
}

void Loader::onLoadFinished(std::shared_ptr<Image> image, QString path) {
    auto task = tasks.take(path);
    delete task;
    if(!image)
        emit loadFailed(path); // due incorrect image format etc
    else
        emit loadFinished(image);
}

void Loader::clearPool() {
    QHashIterator<QString, LoaderRunnable*> i(tasks);
    while (i.hasNext()) {
        i.next();
        if(pool->tryTake(i.value())) {
            delete tasks.take(i.key());
        }
    }
}
