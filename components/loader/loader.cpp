#include "loader.h"

Loader::Loader() {
    pool = new QThreadPool(this);
    pool->setMaxThreadCount(2);
}

void Loader::clearTasks() {
    pool->clear();
    pool->waitForDone();
}

void Loader::loadBlocking(QString path) {
    if(bufferedTasks.contains(path)) {
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(path);
    connect(runnable, SIGNAL(finished(std::shared_ptr<Image>, QString)),
            this, SLOT(onLoadFinished(std::shared_ptr<Image>, QString)), Qt::UniqueConnection);
    runnable->setAutoDelete(true);
    bufferedTasks.append(path);
    runnable->run();
}

// clears all buffered tasks before loading
void Loader::loadExclusive(QString path) {
    if(bufferedTasks.contains(path)) {
        return;
    }
    bufferedTasks.clear();
    pool->clear();
    load(path);
}

void Loader::load(QString path) {
    if(bufferedTasks.contains(path)) {
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(path);
    connect(runnable, SIGNAL(finished(std::shared_ptr<Image>, QString)),
            this, SLOT(onLoadFinished(std::shared_ptr<Image>, QString)), Qt::UniqueConnection);
    runnable->setAutoDelete(true);
    bufferedTasks.append(path);
    pool->start(runnable);
}

void Loader::onLoadFinished(std::shared_ptr<Image> image, QString path) {
    bufferedTasks.removeOne(path);
    if(!image)
        emit loadFailed(path); // due incorrect image format etc
    else
        emit loadFinished(image);
}
