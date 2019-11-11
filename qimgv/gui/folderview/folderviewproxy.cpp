#include "folderviewproxy.h"

FolderViewProxy::FolderViewProxy(QWidget *parent)
    : QWidget(parent),
      folderView(nullptr)
{
    stateBuf.sortingMode = settings->sortingMode();
    layout.setContentsMargins(0,0,0,0);
    mWrapper.reset(new DirectoryViewWrapper(this));
}

void FolderViewProxy::init() {
    if(folderView)
        return;
    folderView.reset(new FolderView());
    folderView->setParent(this);
    layout.addWidget(folderView.get());
    this->setFocusProxy(folderView.get());
    this->setLayout(&layout);
    folderView->show();

    // apply buffer
    if(!stateBuf.directory.isEmpty())
        folderView->setDirectoryPath(stateBuf.directory);
    folderView->setExitButtonEnabled(stateBuf.exitBtn);
    folderView->populate(stateBuf.itemCount);
    folderView->selectIndex(stateBuf.selectedIndex);
    // wait till layout stuff happens
    // before calling focusOn()
    qApp->processEvents();
    folderView->focusOn(stateBuf.selectedIndex);
    folderView->onSortingChanged(stateBuf.sortingMode);

    connect(folderView.get(), &FolderView::thumbnailPressed, this, &FolderViewProxy::thumbnailPressed);
    connect(folderView.get(), &FolderView::thumbnailsRequested, this, &FolderViewProxy::thumbnailsRequested);
    connect(folderView.get(), &FolderView::sortingSelected, this, &FolderViewProxy::sortingSelected);
}

std::shared_ptr<DirectoryViewWrapper> FolderViewProxy::wrapper() {
    return mWrapper;
}

void FolderViewProxy::populate(int count) {
    if(folderView) {
        folderView->populate(count);
    } else {
        stateBuf.itemCount = count;
    }
}

void FolderViewProxy::setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) {
    if(folderView) {
        folderView->setThumbnail(pos, thumb);
    }
}

void FolderViewProxy::selectIndex(int index) {
    if(folderView) {
        folderView->selectIndex(index);
    } else {
        stateBuf.selectedIndex = index;
    }
}

int FolderViewProxy::selectedIndex() {
    if(folderView) {
        return folderView->selectedIndex();
    } else {
        return stateBuf.selectedIndex;
    }
}

void FolderViewProxy::focusOn(int index) {
    if(folderView) {
        folderView->focusOn(index);
    }
}

void FolderViewProxy::setDirectoryPath(QString path) {
    if(folderView) {
        folderView->setDirectoryPath(path);
    } else {
        stateBuf.directory = path;
    }
}

void FolderViewProxy::insertItem(int index) {
    if(folderView) {
        folderView->insertItem(index);
    } else {
        stateBuf.itemCount++;
    }
}

void FolderViewProxy::removeItem(int index) {
    if(folderView) {
        folderView->removeItem(index);
    } else {
        stateBuf.itemCount--;
    }
}

void FolderViewProxy::reloadItem(int index) {
    if(folderView)
        folderView->reloadItem(index);
}

void FolderViewProxy::addItem() {
    if(folderView) {
        folderView->addItem();
    } else {
        stateBuf.itemCount++;
    }
}

void FolderViewProxy::setExitButtonEnabled(bool mode) {
    if(folderView) {
        folderView->setExitButtonEnabled(mode);
    } else {
        stateBuf.exitBtn = mode;
    }
}

void FolderViewProxy::onSortingChanged(SortingMode mode) {
    if(folderView) {
        folderView->onSortingChanged(mode);
    } else {
        stateBuf.sortingMode = mode;
    }
}

void FolderViewProxy::showEvent(QShowEvent *event) {
    init();
    QWidget::showEvent(event);
}
