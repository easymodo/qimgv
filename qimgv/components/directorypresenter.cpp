#include "directorypresenter.h"

DirectoryPresenter::DirectoryPresenter(QObject *parent) : QObject(parent) {
}

void DirectoryPresenter::unsetModel() {
    disconnect(model.get(), SIGNAL(fileRemoved(QString, int)),
               this, SLOT(onFileRemoved(QString, int)));
    disconnect(model.get(), SIGNAL(fileAdded(QString)),
               this, SLOT(onFileAdded(QString)));
    disconnect(model.get(), SIGNAL(fileModified(QString)),
               this, SLOT(onFileModified(QString)));
    disconnect(model.get(), SIGNAL(fileRenamed(QString, int, QString, int)),
               this, SLOT(onFileRenamed(QString, int, QString, int)));
    disconnect(model.get(), SIGNAL(directoryChanged(QString)),
               this, SLOT(reloadModel()));
    disconnect(model.get(), SIGNAL(sortingChanged()),
               this, SLOT(onModelSortingChanged()));
    disconnect(model.get(), &DirectoryModel::thumbnailReady,
               this,  &DirectoryPresenter::onThumbnailReady);
    disconnect(model.get(), SIGNAL(indexChanged(int)),
               this, SLOT(setCurrentIndex(int)));
    model = nullptr;
    // also empty views?
}

void DirectoryPresenter::setModel(std::shared_ptr<DirectoryModel> newModel) {
    if(model)
        unsetModel();
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
    connect(model.get(), SIGNAL(fileRenamed(QString, int, QString, int)),
            this, SLOT(onFileRenamed(QString, int, QString, int)), Qt::UniqueConnection);

    connect(model.get(), SIGNAL(indexChanged(int)),
            this, SLOT(setCurrentIndex(int)), Qt::UniqueConnection);

    connect(model.get(), SIGNAL(directoryChanged(QString)),
            this, SLOT(reloadModel()), Qt::UniqueConnection);

    connect(model.get(), SIGNAL(sortingChanged()),
            this, SLOT(onModelSortingChanged()), Qt::UniqueConnection);

    connect(this,        &DirectoryPresenter::generateThumbnails,
            model.get(), &DirectoryModel::generateThumbnails, Qt::UniqueConnection);
    connect(model.get(), &DirectoryModel::thumbnailReady,
            this,        &DirectoryPresenter::onThumbnailReady, Qt::UniqueConnection);
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
    }
}

void DirectoryPresenter::disconnectAllViews() {
    for(int i=0; i<views.count(); i++) {
        disconnectView(views.at(i));
    }
}

void DirectoryPresenter::disconnectView(std::shared_ptr<DirectoryViewWrapper> view) {
    // remove items, disconnect
    Q_UNUSED(view)
}

//------------------------------------------------------------------------------

void DirectoryPresenter::onFileRemoved(QString fileName, int index) {
    Q_UNUSED(fileName)

    for(int i=0; i<views.count(); i++) {
        views.at(i)->removeItem(index);
    }
}

void DirectoryPresenter::onFileRenamed(QString from, int indexFrom, QString to, int indexTo) {
    Q_UNUSED(from)
    Q_UNUSED(to)

    for(int i=0; i<views.count(); i++) {
        views.at(i)->removeItem(indexFrom);
        views.at(i)->insertItem(indexTo);
    }
    setCurrentIndex(model->currentIndex());
}

void DirectoryPresenter::onFileAdded(QString fileName) {
    int index = model->indexOf(fileName);
    for(int i=0; i<views.count(); i++) {
        views.at(i)->insertItem(index);
    }
    setCurrentIndex(model->currentIndex());
}

void DirectoryPresenter::onFileModified(QString fileName) {
    Q_UNUSED(fileName)
}

void DirectoryPresenter::onModelSortingChanged() {
    reloadModel();
    setCurrentIndex(model->indexOf(model->currentFileName()));
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
    model->setIndexAsync(index);
}

// tmp -- ?
void DirectoryPresenter::setCurrentIndex(int index) {
    for(int i=0; i<views.count(); i++) {
        views.at(i)->selectIndex(index);
    }
}
