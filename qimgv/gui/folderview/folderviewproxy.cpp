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
    folderView->onFullscreenModeChanged(stateBuf.fullscreenMode);
    folderView->populate(stateBuf.itemCount);
    folderView->selectIndex(stateBuf.selectedIndex);
    // wait till layout stuff happens
    // before calling focusOn()
    qApp->processEvents();
    folderView->focusOn(stateBuf.selectedIndex);
    folderView->onSortingChanged(stateBuf.sortingMode);

    connect(folderView.get(), &FolderView::itemSelected, this, &FolderViewProxy::itemSelected);
    connect(folderView.get(), &FolderView::thumbnailsRequested, this, &FolderViewProxy::thumbnailsRequested);
    connect(folderView.get(), &FolderView::sortingSelected, this, &FolderViewProxy::sortingSelected);
    connect(folderView.get(), &FolderView::directorySelected, this, &FolderViewProxy::directorySelected);
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
        if(index < stateBuf.selectedIndex) {
            stateBuf.selectedIndex--;
        } else if(index == stateBuf.selectedIndex) {
            if(stateBuf.selectedIndex >= stateBuf.itemCount)
                stateBuf.selectedIndex = stateBuf.itemCount - 1;
            else
                stateBuf.selectedIndex = index;
        }
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

void FolderViewProxy::onFullscreenModeChanged(bool mode) {
    if(folderView) {
        folderView->onFullscreenModeChanged(mode);
    } else {
        stateBuf.fullscreenMode = mode;
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
