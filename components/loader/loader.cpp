#include "loader.h"

Loader::Loader() {
}

void Loader::openBlocking(QString path) {
    if(tasks.contains(path)) {
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(path);
    connect(runnable, SIGNAL(finished(Image*)), this, SLOT(onLoadFinished(Image*)), Qt::UniqueConnection);
    runnable->setAutoDelete(true);
    tasks.append(path);
    runnable->run();
}

void Loader::open(QString path) {
    if(tasks.contains(path)) {
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(path);
    connect(runnable, SIGNAL(finished(Image*)), this, SLOT(onLoadFinished(Image*)), Qt::UniqueConnection);
    runnable->setAutoDelete(true);
    tasks.append(path);
    QThreadPool::globalInstance()->start(runnable);
}

void Loader::onLoadFinished(Image *image) {
    tasks.removeAt(tasks.indexOf(image->getPath()));
    emit loadFinished(image);
}
