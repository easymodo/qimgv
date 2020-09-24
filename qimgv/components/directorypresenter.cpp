#include "directorypresenter.h"

DirectoryPresenter::DirectoryPresenter(QObject *parent) : QObject(parent), showDirs(true) {
    connect(&thumbnailer, &Thumbnailer::thumbnailReady, this, &DirectoryPresenter::onThumbnailReady);
}

void DirectoryPresenter::unsetModel() {
    disconnect(model.get(), &DirectoryModel::fileRemoved,    this, &DirectoryPresenter::onFileRemoved);
    disconnect(model.get(), &DirectoryModel::fileAdded,      this, &DirectoryPresenter::onFileAdded);
    disconnect(model.get(), &DirectoryModel::fileRenamed,    this, &DirectoryPresenter::onFileRenamed);
    disconnect(model.get(), &DirectoryModel::fileModified,   this, &DirectoryPresenter::onFileModified);
    disconnect(model.get(), &DirectoryModel::fileModifiedInternal,   this, &DirectoryPresenter::onFileModified);
    model = nullptr;
    // also empty view?
}

void DirectoryPresenter::setView(std::shared_ptr<IDirectoryView> _view) {
    if(view)
        return;
    view = _view;
    if(model)
        view->populate(showDirs ? model->totalCount() : model->fileCount());
    connect(dynamic_cast<QObject *>(view.get()), SIGNAL(itemActivated(int)),
            this, SLOT(onItemActivated(int)));
    connect(dynamic_cast<QObject *>(view.get()), SIGNAL(thumbnailsRequested(QList<int>, int, bool, bool)),
            this, SLOT(generateThumbnails(QList<int>, int, bool, bool)));
}

void DirectoryPresenter::setModel(std::shared_ptr<DirectoryModel> newModel) {
    if(model)
        unsetModel();
    if(!newModel)
        return;
    model = newModel;
    populateView();

    // filesystem changes
    connect(model.get(), &DirectoryModel::fileRemoved,    this, &DirectoryPresenter::onFileRemoved);
    connect(model.get(), &DirectoryModel::fileAdded,      this, &DirectoryPresenter::onFileAdded);
    connect(model.get(), &DirectoryModel::fileRenamed,    this, &DirectoryPresenter::onFileRenamed);
    connect(model.get(), &DirectoryModel::fileModified,   this, &DirectoryPresenter::onFileModified);
    connect(model.get(), &DirectoryModel::fileModifiedInternal,   this, &DirectoryPresenter::onFileModified);
}

void DirectoryPresenter::reloadModel() {
    populateView();
}

void DirectoryPresenter::populateView() {
    if(!model || !view)
        return;
    view->populate(showDirs ? model->totalCount() : model->fileCount());
}

void DirectoryPresenter::disconnectView() {
   // todo
}

//------------------------------------------------------------------------------

void DirectoryPresenter::onFileRemoved(QString filePath, int index) {
    Q_UNUSED(filePath)
    if(!view)
        return;
    view->removeItem(showDirs ? index + model->dirCount() : index);
}

void DirectoryPresenter::onFileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo) {
    Q_UNUSED(fromPath)
    Q_UNUSED(toPath)
    if(!view)
        return;
    if(showDirs) {
        indexFrom += model->dirCount();
        indexTo += model->dirCount();
    }
    // todo: deal with multi-selection
    //int selectedIndex = view->selectedIndex();
    view->removeItem(indexFrom);
    view->insertItem(indexTo);
    //if(selectedIndex == indexFrom || selectedIndex == -1) {
    //    view->select(indexTo);
    //    view->focusOn(indexTo);
    //}
}

void DirectoryPresenter::onFileAdded(QString filePath) {
    if(!view)
        return;
    int index = model->indexOfFile(filePath);
    view->insertItem(showDirs ? model->dirCount() + index : index);
}

void DirectoryPresenter::onFileModified(QString filePath) {
    if(!view)
        return;
    int index = model->indexOfFile(filePath);
    view->reloadItem(showDirs ? model->dirCount() + index : index);
}

void DirectoryPresenter::setShowDirs(bool mode) {
    if(mode == showDirs)
        return;
    showDirs = mode;
    populateView();
}

QList<int> DirectoryPresenter::selection() const {
    if(!view)
        return QList<int>();
    return view->selection();
}

void DirectoryPresenter::generateThumbnails(QList<int> indexes, int size, bool crop, bool force) {
    thumbnailer.clearTasks();
    if(!showDirs) {
        for(int i : indexes)
            thumbnailer.getThumbnailAsync(model->filePathAt(i), size, crop, force);
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
            view->setThumbnail(i, thumb);
        } else {
            QString path = model->filePathAt(i - model->dirCount());
            thumbnailer.getThumbnailAsync(path, size, crop, force);
        }
    }
}

void DirectoryPresenter::onThumbnailReady(std::shared_ptr<Thumbnail> thumb, QString filePath) {
    if(!view)
        return;
    int index = model->indexOfFile(filePath);
    if(index == -1)
        return;
    view->setThumbnail(showDirs ? model->dirCount() + index : index, thumb);
}

void DirectoryPresenter::onItemActivated(int index) {
    if(!showDirs) {
        emit fileActivated(index);
        return;
    }
    if(index < model->dirCount())
        emit dirActivated(model->dirPathAt(index));
    else
        emit fileActivated(index - model->dirCount());
}

void DirectoryPresenter::selectAndFocus(int index) {
    if(!view)
        return;
    int indexDirOffset = showDirs ? model->dirCount() + index : index;
    view->select(QList<int>() << indexDirOffset);
    view->focusOn(indexDirOffset);
}

// TODO: in future this will behave differently when the view has multi-selection (not implemented yet)
void DirectoryPresenter::onIndexChanged(int index) {
    this->selectAndFocus(index);
}
