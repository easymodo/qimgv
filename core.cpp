#include "core.h"

Core::Core() : QObject() {
    initVariables();
    connectSlots();
    initSettings();
    QPixmapCache::setCacheLimit(100);
}

void Core::initVariables() {
    imageViewer = new ImageViewer();
    dirManager = new DirectoryManager();
    imgLoader = new ImageLoader(dirManager);
    openDialog = new OpenDialog();
}

// misc connections not related to gui
void Core::connectSlots() {
    connect(dirManager, SIGNAL(directoryChanged(QString)), this, SLOT(setDialogDir(QString)));
}

//default settings, more to go
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
    connect(mainWindow, SIGNAL(signalFitAll()), imageViewer, SLOT(fitAll()));
    connect(mainWindow, SIGNAL(signalFitWidth()), imageViewer, SLOT(fitWidth()));
    connect(mainWindow, SIGNAL(signalFitNormal()), imageViewer, SLOT(fitNormal()));
    connect(mainWindow, SIGNAL(signalZoomIn()), this, SLOT(slotZoomIn()));
    connect(mainWindow, SIGNAL(signalZoomOut()), this, SLOT(slotZoomOut()));
    connect(imageViewer, SIGNAL(sendDoubleClick()), mainWindow, SLOT(slotTriggerFullscreen()));
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
    imageViewer->displayImage(imgLoader->loadNext());
    //Image *img = imgLoader->loadNext();
   // QPixmap *img1 = new QPixmap("K:/_code/sample images/New folder/8.jpg");
    //QPixmapCache::remove(img1->cacheKey());
    //img1->~QPixmap();
    //QPixmapCache::clear();
}

void Core::slotPrevImage() {
    imageViewer->displayImage(imgLoader->loadPrev());
    //QPixmap *img1 = new QPixmap("K:/_code/sample images/New folder/9.jpg");
    //img1=dynamic_cast<QPixmap&>(img1);
    //QPixmapCache::insert(dynamic_cast<const QPixmap&>(img1));
    //img1->~QPixmap();
    //QPixmapCache::clear();
}

void Core::updateOverlays() {
    //todo
}

void Core::open(QString filePath) {
    imageViewer->displayImage(imgLoader->load(filePath));
}
