#include "directoryviewwrapper.h"

DirectoryViewWrapper::DirectoryViewWrapper(QObject *parent) : QObject(parent) {
    Q_ASSERT(parent);
    view = dynamic_cast<IDirectoryView*>(parent);
    Q_ASSERT(view);

    connect(parent, SIGNAL(thumbnailPressed(int)), this, SIGNAL(thumbnailPressed(int)));
    connect(parent, SIGNAL(thumbnailsRequested(QList<int>, int)), this, SIGNAL(thumbnailsRequested(QList<int>, int)));
}

void DirectoryViewWrapper::populate(int count) {
    view->populate(count);
}

void DirectoryViewWrapper::setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) {
    view->setThumbnail(pos, thumb);
}

void DirectoryViewWrapper::selectIndex(int index) {
    view->selectIndex(index);
}

int DirectoryViewWrapper::selectedIndex() {
    return view->selectedIndex();
}

void DirectoryViewWrapper::focusOn(int index) {
    view->focusOn(index);
}

void DirectoryViewWrapper::setDirectoryPath(QString path) {
    view->setDirectoryPath(path);
}

void DirectoryViewWrapper::insertItem(int index) {
    view->insertItem(index);
}

void DirectoryViewWrapper::removeItem(int index) {
    view->removeItem(index);
}
