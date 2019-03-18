#include "directorypresenter.h"

DirectoryPresenter::DirectoryPresenter(QObject *parent) : QObject(parent) {
}

void DirectoryPresenter::removeModel() {
    disconnect(model.get(), SIGNAL(fileRemoved(QString, int)),
               this, SLOT(onFileRemoved(QString, int)));
    disconnect(model.get(), SIGNAL(fileAdded(QString)),
               this, SLOT(onFileAdded(QString)));
    disconnect(model.get(), SIGNAL(fileModified(QString)),
               this, SLOT(onFileModified(QString)));
    disconnect(model.get(), SIGNAL(fileRenamed(QString, QString)),
               this, SLOT(onFileRenamed(QString, QString)));
    disconnect(model.get(), SIGNAL(directoryChanged(QString)),
               this, SLOT(reloadModel()));
    disconnect(model->thumbnailer, &Thumbnailer::thumbnailReady,
               this, &DirectoryPresenter::onThumbnailReady);
    model = nullptr;
    // also empty views?
}

void DirectoryPresenter::setModel(std::shared_ptr<DirectoryModel> newModel) {
    if(model)
        removeModel();
    if(!newModel)
        return;
    model = newModel;
    // repopulate views, load thumbnails? maybe save visible item list in view
    for(int i=0; i<views.count(); i++) {
        views.at(i)->populate(model->itemCount());
    }
    // filesystem changes
    connect(model.get(), SIGNAL(fileRemoved(QString, int)),
            this, SLOT(onFileRemoved(QString, int)), Qt::UniqueConnection);
    connect(model.get(), SIGNAL(fileAdded(QString)),
            this, SLOT(onFileAdded(QString)), Qt::UniqueConnection);
    connect(model.get(), SIGNAL(fileModified(QString)),
            this, SLOT(onFileModified(QString)), Qt::UniqueConnection);
    connect(model.get(), SIGNAL(fileRenamed(QString, QString)),
            this, SLOT(onFileRenamed(QString, QString)), Qt::UniqueConnection);

    connect(model.get(), SIGNAL(directoryChanged(QString)),
            this, SLOT(reloadModel()), Qt::UniqueConnection);

    connect(this, &DirectoryPresenter::generateThumbnails,
            model->thumbnailer, &Thumbnailer::generateThumbnails, Qt::UniqueConnection);
    connect(model->thumbnailer, &Thumbnailer::thumbnailReady,
            this, &DirectoryPresenter::onThumbnailReady, Qt::UniqueConnection);
}

void DirectoryPresenter::connectView(std::shared_ptr<DirectoryViewWrapper> view) {
    if(view && !views.contains(view)) {
        views.append(view);
        if(model)
            view->populate(model->itemCount());
        // todo: connect to presenter only!! passthrough signals / slots
        connect(view.get(), &DirectoryViewWrapper::thumbnailPressed,
                this, &DirectoryPresenter::loadByIndex, Qt::UniqueConnection);
        connect(view.get(), &DirectoryViewWrapper::thumbnailsRequested,
                this, &DirectoryPresenter::generateThumbnails, Qt::UniqueConnection);

        /*connect(view.get(), SIGNAL(thumbnailsRequested(QList<int>, int)),
                model->thumbnailer, SLOT(generateThumbnailFor(QList<int>, int)), Qt::UniqueConnection);

        connect(model->thumbnailer, SIGNAL(thumbnailReady(std::shared_ptr<Thumbnail>)),
                this, SLOT(forwardThumbnail(std::shared_ptr<Thumbnail>)));

        connect(this, SIGNAL(currentIndexChanged(int)),
                view.get(), SIGNAL(setCurrentIndex(int)));

        connect(view.get(), SIGNAL(thumbnailPressed(int)),
                this, SLOT(loadByIndex(int)));
                */
    }
}

void DirectoryPresenter::disconnectAllViews() {
    for(int i=0; i<views.count(); i++) {
        disconnectView(views.at(i));
    }
}

void DirectoryPresenter::disconnectView(std::shared_ptr<DirectoryViewWrapper> view) {
    // remove items, disconnect
}

//------------------------------------------------------------------------------

void DirectoryPresenter::onFileRemoved(QString fileName, int index) {
    for(int i=0; i<views.count(); i++) {
        views.at(i)->removeItem(index);
    }
}

void DirectoryPresenter::onFileRenamed(QString from, QString to) {

}

void DirectoryPresenter::onFileAdded(QString fileName) {
    int index = model->indexOf(fileName);
    for(int i=0; i<views.count(); i++) {
        views.at(i)->insertItem(index);
    }
    setCurrentIndex(model->indexOf(model->currentFileName));
}

void DirectoryPresenter::onFileModified(QString fileName) {

}

void DirectoryPresenter::reloadModel() {
    if(model)
        setModel(model);
}

void DirectoryPresenter::onThumbnailReady(std::shared_ptr<Thumbnail> thumb) {
    int index = model->indexOf(thumb->name());
    for(int i=0; i<views.count(); i++) {
        views.at(i)->setThumbnail(index, thumb);
    }
}

void DirectoryPresenter::loadByIndex(int index) {
    model->setIndex(index);
    if(index >= 0 && index < model->itemCount()) {
        model->currentFileName = model->fileNameAt(index);
        //onLoadStarted();
        // First check if image is already cached. If it is, just display it.
        // force reload??
        //if(model->cache.contains(model->currentFileName)) {
        //    displayImage(model->cache.get(model->currentFileName).get());
        //} else {
            model->loader.loadExclusive(model->fullFilePath(model->currentFileName));
        //}
        //preload(model->dirManager.prevOf(model->currentFileName));
        //preload(model->dirManager.nextOf(model->currentFileName));
        //return true;
    }
    //return false;
}

// tmp
void DirectoryPresenter::setCurrentIndex(int index) {
    for(int i=0; i<views.count(); i++) {
        views.at(i)->selectIndex(index);
    }
}
