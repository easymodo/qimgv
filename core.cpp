#include "core.h"

Core::Core(QWidget *_parent) :
    QObject(_parent)
{
    scrollArea = new ScrollArea();
    dirManager = new DirectoryManager();
    parent = _parent;
    dirManager->changeCurrentDir(tr("C:\\"));
   // connect(scrollArea, SIGNAL(exitClicked()), this, SLOT(close()));
   // connect(scrollArea, SIGNAL(exitFullscreenClicked()), this, SLOT(triggerFullscreen()));
   // connect(scrollArea, SIGNAL(minimizeClicked()), this, SLOT(minimizeWindow()));

//     connect(mImageView, SIGNAL(resized()), this, SLOT(updateMapOverlay()));

   // connect(scrollArea, SIGNAL(scrollbarChanged()), this, SLOT(updateMapOverlay()));
   // connect(scrollArea, SIGNAL(sendDoubleClick()), this, SLOT(triggerFullscreen()));
}

QWidget* Core::getMainWidget() {
    return scrollArea;
}


void Core::showNextImage() {

}

void Core::showPrevImage() {

}

void Core::updateOverlays() {

}

void Core::open(QString filePath)
{
    scrollArea->setImagePath(filePath);
}

void Core::openDialog() {
    QString imagesFilter = tr("Images (*.png *.jpg *jpeg *bmp *gif)");
    //QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),
    //        currentDir.currentPath(), imagesFilter, 0);//,QFileDialog::DontUseNativeDialog);
    QString filePath = QFileDialog::getOpenFileName(parent, tr("Open File"),tr("C:\\"), imagesFilter, 0);
    open(filePath);
}
