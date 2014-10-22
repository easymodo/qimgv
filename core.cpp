#include "core.h"

Core::Core() :
    QObject(),
    mainWindow(NULL),
    imageViewer(NULL),
    imgLoader(NULL),
    openDialog(NULL),
    dirManager(NULL)
{
    initVariables();
    connectSlots();
    initSettings();
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
    connect(imageViewer, SIGNAL(imageChanged()), this, SLOT(setInfoString()));
    connect(imageViewer, SIGNAL(imageChanged()), imgLoader, SLOT(deleteLastImage()));
}

//default settings, more to go
void Core::initSettings() {
    dirManager->setCurrentDir(tr("K:/_code/sao_test/"));
}

void Core::connectGui(MainWindow *mw) {
    mainWindow = mw;
    mainWindow->setCentralWidget(imageViewer);
    openDialog->setParent(mainWindow);
    imageViewer->setParent(mainWindow);
    connect(mainWindow, SIGNAL(signalOpenDialog()), this, SLOT(showOpenDialog()));
    connect(mainWindow, SIGNAL(signalNextImage()), this, SLOT(slotNextImage()));
    connect(mainWindow, SIGNAL(signalPrevImage()), this, SLOT(slotPrevImage()));
    connect(mainWindow, SIGNAL(signalFitAll()), imageViewer, SLOT(slotFitAll()));
    connect(mainWindow, SIGNAL(signalFitWidth()), imageViewer, SLOT(slotFitWidth()));
    connect(mainWindow, SIGNAL(signalFitNormal()), imageViewer, SLOT(slotFitNormal()));
    connect(mainWindow, SIGNAL(signalZoomIn()), imageViewer, SLOT(slotZoomIn()));
    connect(mainWindow, SIGNAL(signalZoomOut()), imageViewer, SLOT(slotZoomOut()));
    connect(imageViewer, SIGNAL(sendDoubleClick()), mainWindow, SLOT(slotTriggerFullscreen()));
    connect(mainWindow, SIGNAL(signalFullscreenEnabled(bool)), imageViewer, SLOT(slotShowControls(bool)));
    connect(mainWindow, SIGNAL(signalFullscreenEnabled(bool)), imageViewer, SLOT(slotShowInfo(bool)));
    connect(imageViewer->getControls(), SIGNAL(exitClicked()), mainWindow, SLOT(close()));
    connect(imageViewer->getControls(), SIGNAL(exitFullscreenClicked()), mainWindow, SLOT(slotTriggerFullscreen()));
    connect(imageViewer->getControls(), SIGNAL(minimizeClicked()), mainWindow, SLOT(slotMinimize()));
    connect(imageViewer->getControls(), SIGNAL(minimizeClicked()), mainWindow, SLOT(slotMinimize()));
}

void Core::setInfoString() {
    Image *i = imageViewer->getImage();
    QString infoString = "";
    infoString.append(i->getName() + "  ");
    infoString.append("(" +
                      QString::number(i->width()) +
                      "x" +
                      QString::number(i->height()) +
                      ")  ");
    infoString.append("[ " +
                      QString::number(i->getInfo().getCurrentPos()) +
                      "/" +
                      QString::number(i->getInfo().getMaxPos()) +
                      " ] ");
    imageViewer->slotSetInfoString(infoString);
    if(mainWindow != NULL) {
        infoString.append("- qimgv");
        mainWindow->setWindowTitle(infoString);
    }
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
    imageViewer->setImage(imgLoader->loadNext());
}

void Core::slotPrevImage() {
    imageViewer->setImage(imgLoader->loadPrev());

}

void Core::open(QString filePath) {
    imageViewer->setImage(imgLoader->load(filePath));
}
