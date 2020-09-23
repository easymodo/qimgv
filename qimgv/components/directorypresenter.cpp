#include "directorypresenter.h"

// todo: im just temporarily piling everything here.  use MVVM instead of MVP

DirectoryPresenter::DirectoryPresenter(QObject *parent) : QObject(parent) {
    connect(&thumbnailer, &Thumbnailer::thumbnailReady, this, &DirectoryPresenter::onThumbnailReady);
}

void DirectoryPresenter::unsetModel() {
    disconnect(model.get(), &DirectoryModel::fileRemoved,    this, &DirectoryPresenter::onFileRemoved);
    disconnect(model.get(), &DirectoryModel::fileAdded,      this, &DirectoryPresenter::onFileAdded);
    disconnect(model.get(), &DirectoryModel::fileModified,   this, &DirectoryPresenter::onFileModified);
    disconnect(model.get(), &DirectoryModel::fileModifiedInternal,   this, &DirectoryPresenter::onFileModified);
    disconnect(model.get(), &DirectoryModel::fileRenamed,    this, &DirectoryPresenter::onFileRenamed);
    model = nullptr;
    // also empty views?
}

void DirectoryPresenter::setFolderView(std::shared_ptr<FolderViewProxy> view) {
    if(folderView)
        return;
    folderView = view;
    if(model)
        folderView->populate(settings->directoriesInFolderView() ? model->totalCount() : model->fileCount());
    connect(folderView.get(), &FolderViewProxy::itemActivated,
            this, &DirectoryPresenter::onItemActivatedFolderView);
    connect(folderView.get(), &FolderViewProxy::thumbnailsRequested,
            this, &DirectoryPresenter::generateThumbnailsFolderView);
}

void DirectoryPresenter::setThumbPanel(std::shared_ptr<ThumbnailStripProxy> view) {
    if(thumbPanel)
        return;
    thumbPanel = view;
    if(model)
        view->populate(model->fileCount());
    connect(thumbPanel.get(), &ThumbnailStripProxy::itemActivated,
            this, &DirectoryPresenter::fileActivated);
    connect(thumbPanel.get(), &ThumbnailStripProxy::thumbnailsRequested,
            this, &DirectoryPresenter::generateThumbnails);
}

void DirectoryPresenter::setModel(std::shared_ptr<DirectoryModel> newModel) {
    if(model)
        unsetModel();
    if(!newModel)
        return;
    model = newModel;
    populateViews();

    // filesystem changes
    connect(model.get(), &DirectoryModel::fileRemoved,    this, &DirectoryPresenter::onFileRemoved);
    connect(model.get(), &DirectoryModel::fileAdded,      this, &DirectoryPresenter::onFileAdded);
    connect(model.get(), &DirectoryModel::fileModified,   this, &DirectoryPresenter::onFileModified);
    connect(model.get(), &DirectoryModel::fileModifiedInternal,   this, &DirectoryPresenter::onFileModified);
    connect(model.get(), &DirectoryModel::fileRenamed,    this, &DirectoryPresenter::onFileRenamed);
}

void DirectoryPresenter::reloadModel() {
    populateViews();
}

void DirectoryPresenter::populateViews() {
    if(!model)
        return;
    if(folderView)
        folderView->populate(settings->directoriesInFolderView() ? model->totalCount() : model->fileCount());
    if(thumbPanel)
        thumbPanel->populate(model->fileCount());
}

void DirectoryPresenter::disconnectAllViews() {
   // todo
}

//------------------------------------------------------------------------------

void DirectoryPresenter::onFileRemoved(QString filePath, int index) {
    Q_UNUSED(filePath)
    if(folderView)
        folderView->removeItem(settings->directoriesInFolderView() ? index + model->dirCount() : index);
    if(thumbPanel)
        thumbPanel->removeItem(index);
}

// todo fix dir offset
void DirectoryPresenter::onFileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo) {
    Q_UNUSED(fromPath)
    Q_UNUSED(toPath)

    if(folderView) {
        int selectedIndex = folderView->selectedIndex();
        folderView->removeItem(indexFrom);
        folderView->insertItem(indexTo);
        if(selectedIndex == indexFrom ||
           selectedIndex == -1)
        {
            folderView->selectIndex(indexTo);
            folderView->focusOn(indexTo);
        }
    }
    if(thumbPanel) {
        int selectedIndex = thumbPanel->selectedIndex();
        thumbPanel->removeItem(indexFrom);
        thumbPanel->insertItem(indexTo);
        if(selectedIndex == indexFrom ||
           selectedIndex == -1)
        {
            thumbPanel->selectIndex(indexTo);
            thumbPanel->focusOn(indexTo);
        }
    }
}

void DirectoryPresenter::onFileAdded(QString filePath) {
    int index = model->indexOfFile(filePath);
    if(folderView)
        folderView->insertItem(settings->directoriesInFolderView() ? model->dirCount() + index : index);
    if(thumbPanel)
        thumbPanel->insertItem(index);
}

void DirectoryPresenter::onFileModified(QString filePath) {
    int index = model->indexOfFile(filePath);
    if(folderView)
        folderView->reloadItem(settings->directoriesInFolderView() ? model->dirCount() + index : index);
    if(thumbPanel)
        thumbPanel->reloadItem(index);
}

// [dirs] + files
void DirectoryPresenter::generateThumbnailsFolderView(QList<int> indexes, int size, bool crop, bool force) {
    if(!settings->directoriesInFolderView()) {
        generateThumbnails(indexes, size, crop, force);
        return;
    }
    for(int i : indexes) {
        if(i < model->dirCount()) {
            // gen thumb for a directory (!todo: shared res)
            // tmp ------------------------------------------------------------
            QPixmap *pixmap = new QPixmap(":/res/icons/common/other/folder96.png");
            ImageLib::recolor(*pixmap, settings->colorScheme().text);
            std::shared_ptr<Thumbnail> thumb(new Thumbnail(model->dirNameAt(i),
                                                           "Folder",
                                                           size,
                                                           std::shared_ptr<QPixmap>(pixmap)));
            // ^----------------------------------------------------------------
            folderView->setThumbnail(i, thumb);
        } else {
            QString path = model->filePathAt(i - model->dirCount());
            thumbnailer.getThumbnailAsync(path, size, crop, force);
        }
    }
}

// assumes files only
void DirectoryPresenter::generateThumbnails(QList<int> indexes, int size, bool crop, bool force) {
    thumbnailer.clearTasks();
    for(int i : indexes) {
        thumbnailer.getThumbnailAsync(model->filePathAt(i), size, crop, force);
    }
}

void DirectoryPresenter::onThumbnailReady(std::shared_ptr<Thumbnail> thumb, QString filePath) {
    int index = model->indexOfFile(filePath);
    if(index == -1)
        return;

    if(folderView) {
        folderView->setThumbnail(settings->directoriesInFolderView() ? model->dirCount() + index : index, thumb);
    }
    if(thumbPanel) {
        thumbPanel->setThumbnail(index, thumb);
    }
}

void DirectoryPresenter::onItemActivatedFolderView(int index) {
    if(!settings->directoriesInFolderView()) {
        emit fileActivated(index);
        return;
    }
    if(index < model->dirCount())
        emit dirActivated(model->dirPathAt(index));
    else
        emit fileActivated(index - model->dirCount());
}

void DirectoryPresenter::selectAndFocus(int index) {
    if(folderView)  {
        int indexDirOffset = settings->directoriesInFolderView() ? model->dirCount() + index : index;
        folderView->selectIndex(indexDirOffset);
        folderView->focusOn(indexDirOffset);
    }
    if(thumbPanel) {
        thumbPanel->selectIndex(index);
        thumbPanel->focusOn(index);
    }
}

// TODO: in future this will behave differently when the view has multi-selection (not implemented yet)
void DirectoryPresenter::onIndexChanged(int index) {
    this->selectAndFocus(index);
}
