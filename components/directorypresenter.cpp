#include "directorypresenter.h"

DirectoryPresenter::DirectoryPresenter(QObject *parent) : QObject(parent) {
}

void DirectoryPresenter::removeModel() {

}

void DirectoryPresenter::setModel(std::shared_ptr<DirectoryModel> newModel) {
    if(model)
        removeModel();
    if(!newModel)
        return;
    // repopulate views, load thumbnails? maybe save visible item list in view
    for(int i=0; i<views.count(); i++) {
        //views.at(i)->populate(model->itemCount());
    }
    // filesystem changes
    connect(model.get(), SIGNAL(fileRemoved(QString, int)),     this, SLOT(onFileRemoved(QString, int)));
    connect(model.get(), SIGNAL(fileAdded(QString)),            this, SLOT(onFileAdded(QString)));
    connect(model.get(), SIGNAL(fileModified(QString)),         this, SLOT(onFileModified(QString)));
    connect(model.get(), SIGNAL(fileRenamed(QString, QString)), this, SLOT(onFileRenamed(QString, QString)));
}

void DirectoryPresenter::connectView(std::shared_ptr<IDirectoryView> view) {
    if(view && !views.contains(view)) {
        views.append(view);
        connect(view.get(), &IDirectoryView::thumbnailPressed, this, &DirectoryPresenter::loadByIndex);
        // add connections here
        /*connect(view.get(), SIGNAL(thumbnailRequested(QList<int>, int)),
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

void DirectoryPresenter::disconnectView(std::shared_ptr<IDirectoryView> view) {
    // remove items, disconnect
}

void DirectoryPresenter::loadByIndex(int index) {

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
    int index = model->dirManager.indexOf(fileName);
    for(int i=0; i<views.count(); i++) {
        views.at(i)->insertItem(index);
    }
}

void DirectoryPresenter::onFileModified(QString fileName) {

}
