#include "core.h"

Core::Core() : QObject() {
    initVariables();
    connectSlots();
    initSettings();
}

void Core::initVariables() {
    imageViewer = new ImageViewer2();
    dirManager = new DirectoryManager();
    imgLoader = new ImageLoader(dirManager);
    openDialog = new OpenDialog();
}

void Core::connectSlots() {
    connect(dirManager, SIGNAL(directoryChanged(QString)), this, SLOT(setDialogDir(QString)));
    // connect(scrollArea, SIGNAL(exitClicked()), this, SLOT(close()));
    // connect(scrollArea, SIGNAL(exitFullscreenClicked()), this, SLOT(triggerFullscreen()));
    // connect(scrollArea, SIGNAL(minimizeClicked()), this, SLOT(minimizeWindow()));
    // connect(mImageView, SIGNAL(resized()), this, SLOT(updateMapOverlay()));
    // connect(scrollArea, SIGNAL(scrollbarChanged()), this, SLOT(updateMapOverlay()));
    // connect(scrollArea, SIGNAL(sendDoubleClick()), this, SLOT(triggerFullscreen()));
}

void Core::initSettings() {
    dirManager->setCurrentDir(tr("C:\\"));
}

void Core::connectGui(MainWindow *mw) {
    mainWindow = mw;
    mainWindow->setCentralWidget(imageViewer);
    openDialog->setParent(mainWindow);
    imageViewer->setParent(mainWindow);
    connect(mainWindow, SIGNAL(signalOpenDialog()), this, SLOT(showOpenDialog()));
    connect(mainWindow, SIGNAL(signalNextImage()), this, SLOT(slotNextImage()));
    connect(mainWindow, SIGNAL(signalPrevImage()), this, SLOT(slotPrevImage()));
    connect(mainWindow, SIGNAL(signalFitAll()), this, SLOT(slotFitAll()));
    connect(mainWindow, SIGNAL(signalFitWidth()), this, SLOT(slotFitWidth()));
    connect(mainWindow, SIGNAL(signalFitNormal()), this, SLOT(slotFitNormal()));
    connect(mainWindow, SIGNAL(signalZoomIn()), this, SLOT(slotZoomIn()));
    connect(mainWindow, SIGNAL(signalZoomOut()), this, SLOT(slotZoomOut()));
}

void Core::setCurrentDir(QString path) {
    dirManager->setCurrentDir(path);
}

void Core::setDialogDir(QString path) {
    openDialog->setDirectory(path);
}

void Core::showOpenDialog() {
    open(openDialog->getOpenFileName());
}

void Core::slotNextImage() {
    dirManager->next();
    qDebug() << dirManager->currentDir.currentPath();
    qDebug() << dirManager->getFile()->getName() << " #" << dirManager->currentPosition;
    qDebug() << dirManager->getFile()->getLastModified();

}

void Core::slotPrevImage() {
    dirManager->prev();
    qDebug() << dirManager->currentDir.currentPath();
    qDebug() << dirManager->getFile()->getName() << " #" << dirManager->currentPosition;
    qDebug() << dirManager->getFile()->getLastModified();
}

void Core::slotFitAll() {
}

void Core::slotFitWidth() {
}

void Core::slotFitNormal() {
}

void Core::slotZoomIn() {
}

void Core::slotZoomOut() {
}

void Core::updateOverlays() {
}

void Core::open(QString filePath) {
    imageViewer->displayImage(imgLoader->load(filePath));
    //scrollArea->setImagePath(filePath);
}
