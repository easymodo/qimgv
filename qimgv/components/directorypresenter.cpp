#include "directorypresenter.h"

DirectoryPresenter::DirectoryPresenter(QObject *parent) : QObject(parent) {
}

void DirectoryPresenter::unsetModel() {
    disconnect(model.get(), &DirectoryModel::fileRemoved,    this, &DirectoryPresenter::onFileRemoved);
    disconnect(model.get(), &DirectoryModel::fileAdded,      this, &DirectoryPresenter::onFileAdded);
    disconnect(model.get(), &DirectoryModel::fileModified,   this, &DirectoryPresenter::onFileModified);
    disconnect(model.get(), &DirectoryModel::fileRenamed,    this, &DirectoryPresenter::onFileRenamed);
    disconnect(model.get(), &DirectoryModel::indexChanged,   this, &DirectoryPresenter::onIndexChanged);
    disconnect(model.get(), &DirectoryModel::loaded,         this, &DirectoryPresenter::reloadModel);
    disconnect(model.get(), &DirectoryModel::sortingChanged, this, &DirectoryPresenter::onModelSortingChanged);
    disconnect(model.get(), &DirectoryModel::thumbnailReady, this, &DirectoryPresenter::onThumbnailReady);
    disconnect(this, &DirectoryPresenter::generateThumbnails, model.get(), &DirectoryModel::generateThumbnails);
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
    connect(model.get(), &DirectoryModel::fileRemoved,    this, &DirectoryPresenter::onFileRemoved);
    connect(model.get(), &DirectoryModel::fileAdded,      this, &DirectoryPresenter::onFileAdded);
    connect(model.get(), &DirectoryModel::fileModified,   this, &DirectoryPresenter::onFileModified);
    connect(model.get(), &DirectoryModel::fileRenamed,    this, &DirectoryPresenter::onFileRenamed);
    connect(model.get(), &DirectoryModel::indexChanged,   this, &DirectoryPresenter::onIndexChanged);
    connect(model.get(), &DirectoryModel::loaded,         this, &DirectoryPresenter::reloadModel);
    connect(model.get(), &DirectoryModel::sortingChanged, this, &DirectoryPresenter::onModelSortingChanged);
    connect(model.get(), &DirectoryModel::thumbnailReady, this, &DirectoryPresenter::onThumbnailReady);
    connect(this, &DirectoryPresenter::generateThumbnails, model.get(), &DirectoryModel::generateThumbnails);
}

void DirectoryPresenter::connectView(std::shared_ptr<DirectoryViewWrapper> view) {
    if(view && !views.contains(view)) {
        views.append(view);
        if(model)
            view->populate(model->itemCount());
        // todo: connect to presenter only!! passthrough signals / slots
        connect(view.get(), &DirectoryViewWrapper::itemSelected,    this, &DirectoryPresenter::loadByIndex);
        connect(view.get(), &DirectoryViewWrapper::thumbnailsRequested, this, &DirectoryPresenter::generateThumbnails);
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
        int selectedIndex = views.at(i)->selectedIndex();
        views.at(i)->removeItem(indexFrom);
        views.at(i)->insertItem(indexTo);
        if(selectedIndex == indexFrom ||
           selectedIndex == -1)
        {
            views.at(i)->selectIndex(indexTo);
            views.at(i)->focusOn(indexTo);
        }
    }
}

void DirectoryPresenter::onFileAdded(QString fileName) {
    int index = model->indexOf(fileName);
    for(int i=0; i<views.count(); i++) {
        views.at(i)->insertItem(index);
    }
}

void DirectoryPresenter::onFileModified(QString fileName) {
    int index = model->indexOf(fileName);
    for(int i=0; i<views.count(); i++) {
        views.at(i)->reloadItem(index);
    }
}

void DirectoryPresenter::onModelSortingChanged() {
    reloadModel();
    setCurrentIndex(model->indexOf(model->currentFileName()));
    focusOn(model->indexOf(model->currentFileName()));
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

void DirectoryPresenter::focusOn(int index) {
    for(int i=0; i<views.count(); i++) {
        views.at(i)->focusOn(index);
    }
}

void DirectoryPresenter::onIndexChanged(int oldIndex, int index) {
    for(int i=0; i<views.count(); i++) {
        views.at(i)->selectIndex(index);
        views.at(i)->focusOn(index);
    }
}
