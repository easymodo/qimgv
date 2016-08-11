#include "mainwindow.h"

MainWindow::MainWindow() :
    imageViewer(NULL),
    videoPlayer(NULL),
    panel(NULL),
    currentViewer(0),
    currentDisplay(0),
    layout(NULL),
    borderlessEnabled(false),
    desktopWidget(NULL)
{
    resize(1100, 700);
    setMinimumSize(QSize(400, 300));
    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    init();
    setWindowTitle(QCoreApplication::applicationName() +
                   " " +
                   QCoreApplication::applicationVersion());
}

void MainWindow::init() {
    desktopWidget = QApplication::desktop();

    imageViewer = new ImageViewer(this);
    imageViewer->hide();
    videoPlayer = new VideoPlayer(this);
    videoPlayer->hide();

    QWidget *central = new QWidget();
    controlsOverlay = new ControlsOverlay(imageViewer);
    controlsOverlay->hide();
    infoOverlay = new textOverlay(imageViewer);

    layout = new QVBoxLayout;
    central->setAttribute(Qt::WA_MouseTracking);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    central->setSizePolicy(sizePolicy);
    layout->setContentsMargins(0, 0, 0, 0);
    central->setLayout(layout);
    this->setCentralWidget(central);

    core = new Core();

    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));

    enableImageViewer();

    connect(this, SIGNAL(signalNextImage()),
            core, SLOT(slotNextImage()));

    connect(this, SIGNAL(signalPrevImage()),
            core, SLOT(slotPrevImage()));

    connect(this, SIGNAL(fileOpened(QString)),
            core, SLOT(loadImage(QString)));

    connect(this, SIGNAL(signalFullscreenEnabled(bool)),
            this, SLOT(slotShowControls(bool)));

    connect(this, SIGNAL(signalFullscreenEnabled(bool)),
            this, SLOT(slotShowInfo(bool)));

    connect(core, SIGNAL(infoStringChanged(QString)),
            this, SLOT(setInfoString(QString)));

    connect(controlsOverlay, SIGNAL(exitClicked()),
            this, SLOT(close()));

    connect(controlsOverlay, SIGNAL(exitFullscreenClicked()),
            this, SLOT(slotTriggerFullscreen()));

    connect(controlsOverlay, SIGNAL(minimizeClicked()),
            this, SLOT(slotMinimize()));



    connect(core, SIGNAL(videoChanged(Clip *)),
            this, SLOT(openVideo(Clip *)), Qt::UniqueConnection);

    connect(core, SIGNAL(stopVideo()),
            this, SLOT(disableVideoPlayer()));

    // Shortcuts

    connect(actionManager, SIGNAL(nextImage()), core, SLOT(slotNextImage()));
    connect(actionManager, SIGNAL(prevImage()), core, SLOT(slotPrevImage()));
    connect(actionManager, SIGNAL(fitAll()), this, SLOT(slotFitAll()));
    connect(actionManager, SIGNAL(fitWidth()), this, SLOT(slotFitWidth()));
    connect(actionManager, SIGNAL(fitNormal()), this, SLOT(slotFitNormal()));
    connect(actionManager, SIGNAL(toggleFitMode()), this, SLOT(switchFitMode()));
    connect(actionManager, SIGNAL(toggleFullscreen()), this, SLOT(slotTriggerFullscreen()));
    connect(actionManager, SIGNAL(zoomIn()), imageViewer, SLOT(slotZoomIn()));
    connect(actionManager, SIGNAL(zoomOut()), imageViewer, SLOT(slotZoomOut()));
    connect(actionManager, SIGNAL(resize()), this, SLOT(slotResizeDialog()));
    connect(actionManager, SIGNAL(rotateLeft()), this, SLOT(slotRotateLeft()));
    connect(actionManager, SIGNAL(rotateRight()), this, SLOT(slotRotateRight()));
    connect(actionManager, SIGNAL(openSettings()), this, SLOT(showSettings()));
    connect(actionManager, SIGNAL(crop()), this, SLOT(slotCrop()));
    connect(actionManager, SIGNAL(setWallpaper()), this, SLOT(slotSelectWallpaper()));
    connect(actionManager, SIGNAL(open()), this, SLOT(slotOpenDialog()));
    connect(actionManager, SIGNAL(save()), this, SLOT(slotSaveDialog()));
    connect(actionManager, SIGNAL(exit()), this, SLOT(close()));

    connect(this, SIGNAL(fileSaved(QString)), core, SLOT(saveImage(QString)));

    readSettingsInitial();

    core->init();
}

void MainWindow::enablePanel() {
    if(!panel) {
        panel = new ThumbnailStrip(this);
    }

    connect(this, SIGNAL(resized(QSize)), panel, SLOT(parentResized(QSize)));

    connect(core, SIGNAL(imageChanged(int)),
            panel, SLOT(selectThumbnail(int)), Qt::UniqueConnection);

    connect(panel, SIGNAL(thumbnailClicked(int)),
            core, SLOT(loadImageByPos(int)), Qt::UniqueConnection);

    connect(panel, SIGNAL(thumbnailRequested(int)),
            core, SIGNAL(thumbnailRequested(int)), Qt::UniqueConnection);

    connect(core, SIGNAL(thumbnailReady(int, Thumbnail *)),
            panel, SLOT(setThumbnail(int, Thumbnail *)), Qt::UniqueConnection);

    connect(core, SIGNAL(cacheInitialized(int)),
            panel, SLOT(fillPanel(int)), static_cast<Qt::ConnectionType>(Qt::DirectConnection | Qt::UniqueConnection));

    connect(panel, SIGNAL(panelSizeChanged()),
               this, SLOT(calculatePanelTriggerArea()), Qt::UniqueConnection);

    connect(this, SIGNAL(signalFullscreenEnabled(bool)),
            panel, SLOT(enableWindowControls(bool)), Qt::UniqueConnection);

    connect(panel, SIGNAL(openClicked()), this, SLOT(slotOpenDialog()), Qt::UniqueConnection);
    connect(panel, SIGNAL(saveClicked()), this, SLOT(slotSaveDialog()), Qt::UniqueConnection);
    connect(panel, SIGNAL(settingsClicked()), this, SLOT(showSettings()), Qt::UniqueConnection);
    connect(panel, SIGNAL(exitClicked()), this, SLOT(close()), Qt::UniqueConnection);

    panel->parentResized(size());

    panel->fillPanel(core->imageCount());
}

void MainWindow::disablePanel() {
    if(!panel) {
        return;
    }
    disconnect(core, SIGNAL(imageChanged(int)),
            panel, SLOT(selectThumbnail(int)));

    disconnect(panel, SIGNAL(thumbnailClicked(int)),
            core, SLOT(loadImageByPos(int)));

    disconnect(panel, SIGNAL(thumbnailRequested(int)),
            core, SIGNAL(thumbnailRequested(int)));

    disconnect(core, SIGNAL(thumbnailReady(int, Thumbnail *)),
            panel, SLOT(setThumbnail(int, Thumbnail *)));

    disconnect(core, SIGNAL(cacheInitialized(int)),
            panel, SLOT(fillPanel(int)));

    disconnect(panel, SIGNAL(panelSizeChanged()),
               this, SLOT(calculatePanelTriggerArea()));

    disconnect(this, SIGNAL(signalFullscreenEnabled(bool)),
            panel, SLOT(enableWindowControls(bool)));

    disconnect(panel, SIGNAL(openClicked()), this, SLOT(slotOpenDialog()));
    disconnect(panel, SIGNAL(saveClicked()), this, SLOT(slotSaveDialog()));
    disconnect(panel, SIGNAL(settingsClicked()), this, SLOT(showSettings()));
    disconnect(panel, SIGNAL(exitClicked()), this, SLOT(close()));

}

void MainWindow::enableImageViewer() {
    if(currentViewer != 1) {
        disableVideoPlayer();
        controlsOverlay->setParent(imageViewer);
        infoOverlay->setParent(imageViewer);
        layout->addWidget(imageViewer);

        imageViewer->show();

        connect(imageViewer, SIGNAL(scalingRequested(QSize)),
                core, SLOT(rescaleForZoom(QSize)), Qt::UniqueConnection);

        connect(core, SIGNAL(scalingFinished(QPixmap *)),
                imageViewer, SLOT(updateImage(QPixmap *)), Qt::UniqueConnection);

        connect(core, SIGNAL(frameChanged(QPixmap *)),
                imageViewer, SLOT(updateImage(QPixmap *)),
                static_cast<Qt::ConnectionType>(Qt::DirectConnection | Qt::UniqueConnection));
        // reload after image edits
        connect(core, SIGNAL(imageAltered(QPixmap *)),
                imageViewer, SLOT(displayImage(QPixmap *)), Qt::UniqueConnection);

        connect(core, SIGNAL(videoAltered(Clip *)),
                videoPlayer, SLOT(displayVideo(Clip *)), Qt::UniqueConnection);

        connect(imageViewer, SIGNAL(cropSelected(QRect)),
                core, SLOT(crop(QRect)), Qt::UniqueConnection);

        connect(imageViewer, SIGNAL(wallpaperSelected(QRect)),
                core, SLOT(setWallpaper(QRect)), Qt::UniqueConnection);

        connect(core, SIGNAL(signalSetImage(QPixmap *)),
                this, SLOT(openImage(QPixmap *)), Qt::UniqueConnection);

        connect(this, SIGNAL(signalFitAll()),
                imageViewer, SLOT(slotFitAll()), Qt::UniqueConnection);

        connect(this, SIGNAL(signalFitWidth()),
                imageViewer, SLOT(slotFitWidth()), Qt::UniqueConnection);

        connect(this, SIGNAL(signalFitNormal()),
                imageViewer, SLOT(slotFitNormal()), Qt::UniqueConnection);

        connect(this, SIGNAL(signalZoomIn()),
                imageViewer, SLOT(slotZoomIn()), Qt::UniqueConnection);

        connect(this, SIGNAL(signalZoomOut()),
                imageViewer, SLOT(slotZoomOut()), Qt::UniqueConnection);

        updateOverlays();
        currentViewer = 1;
    }
}

void MainWindow::disableImageViewer() {
    layout->removeWidget(imageViewer);

    disconnect(imageViewer, SIGNAL(scalingRequested(QSize)),
               core, SLOT(rescaleForZoom(QSize)));

    disconnect(imageViewer, SIGNAL(cropSelected(QRect)),
               core, SLOT(crop(QRect)));

    disconnect(imageViewer, SIGNAL(wallpaperSelected(QRect)),
               core, SLOT(setWallpaper(QRect)));

    disconnect(core, SIGNAL(signalSetImage(QPixmap *)),
               imageViewer, SLOT(displayImage(QPixmap *)));

    disconnect(this, SIGNAL(signalZoomIn()),
               imageViewer, SLOT(slotZoomIn()));

    disconnect(this, SIGNAL(signalZoomOut()),
               imageViewer, SLOT(slotZoomOut()));

    currentViewer = 0;
    imageViewer->hide();
}

void MainWindow::enableVideoPlayer() {
    if(currentViewer != 2) {
        connect(this, SIGNAL(resized(QSize)),
                videoPlayer, SIGNAL(parentResized(QSize)), Qt::UniqueConnection);
        disableImageViewer();
        controlsOverlay->setParent(videoPlayer);
        infoOverlay->setParent(videoPlayer);
        layout->addWidget(videoPlayer);
        currentViewer = 2;
        videoPlayer->show();
        updateOverlays();
    }
}

void MainWindow::disableVideoPlayer() {
    layout->removeWidget(videoPlayer);
    videoPlayer->stop();
    disconnect(this, SIGNAL(resized(QSize)),
               videoPlayer, SIGNAL(parentResized(QSize)));
    currentViewer = 0;
    videoPlayer->hide();
}

void MainWindow::openVideo(Clip *clip) {
    enableVideoPlayer();
    videoPlayer->displayVideo(clip);
}

void MainWindow::open(QString path) {
    core->loadImageBlocking(path);
}

void MainWindow::openImage(QPixmap *pixmap) {
    enableImageViewer();
    imageViewer->displayImage(pixmap);
}

void MainWindow::readSettingsInitial() {
    readSettings();
    currentDisplay = settings->lastDisplay();
    if(!settings->fullscreenMode()) {
        restoreWindowGeometry();
    }
}

void MainWindow::readSettings() {
    borderlessEnabled = settings->fullscreenTaskbarShown();
    panelPosition = settings->panelPosition();
    fullscreen = settings->fullscreenMode();
    fitMode = settings->imageFitMode();
    if(fitMode == 1) {
        slotFitWidth();
    } else if(fitMode == 2) {
        slotFitNormal();
    } else {
        slotFitAll();
    }
    emit resized(size());
    settings->panelEnabled()?enablePanel():disablePanel();
    calculatePanelTriggerArea();
}

void MainWindow::calculatePanelTriggerArea() {
    if(!settings->panelEnabled()) {
        panelArea.setRect(0,0,0,0);
        return;
    }
    switch(panelPosition) {
        case LEFT:
            panelArea.setRect(0, 0, panel->width() - 1, height());
            break;
        case RIGHT:
            panelArea.setRect(width() - panel->width() + 1, 0, width(), height());
            break;
        case BOTTOM:
            panelArea.setRect(0, height() - panel->height() + 1, width() - 180, height());
            break;
        case TOP:
            panelArea.setRect(0, 0, width(), panel->height() - 1);
            break;
        }
}

void MainWindow::updateOverlays() {
    controlsOverlay->updateSize(this->centralWidget()->size());
    infoOverlay->updateWidth(this->centralWidget()->width());
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    if(panel) {
        emit resized(size());
    }
    updateOverlays();
}

//#############################################################
//######################### EVENTS ############################
//#############################################################

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() != Qt::RightButton && event->buttons() != Qt::LeftButton) {
        if(panelArea.contains(event->pos()) &&
           panel &&
           !panelArea.contains(lastMouseMovePos))
        {
            panel->show();
        }
        event->ignore();
    }
    lastMouseMovePos = event->pos();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e) {
    if(e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    // check for our needed mime type, here a file or a list of files
    if(mimeData->hasUrls()) {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        // extract the local paths of the files
        for(int i = 0; i < urlList.size() && i < 32; ++i) {
            pathList.append(urlList.at(i).toLocalFile());
        }
        // try to open first file in the list
        open(pathList.first());
    }
}

bool MainWindow::event(QEvent *event) {
    return (actionManager->processEvent(event)) ? true : QMainWindow::event(event);
}


void MainWindow::closeEvent(QCloseEvent *event) {
    this->hide();
    if(QThreadPool::globalInstance()->activeThreadCount()) {
        QThreadPool::globalInstance()->waitForDone();
    }
    if(!isMaximized() && !isFullScreen()) {
        saveWindowGeometry();
    }
    saveDisplay();
    QMainWindow::closeEvent(event);
}

void MainWindow::saveDisplay() {
    if(settings->fullscreenMode() || isFullScreen()) {
        settings->setLastDisplay(desktopWidget->screenNumber(this));
    }
}

void MainWindow::saveWindowGeometry() {
    #ifdef __linux__
    settings->setWindowGeometry(QRect(pos(), size()));
    //settings->setWindowGeometry(geometry());
    #else
         settings->setWindowGeometry(QRect(pos(), size()));
    #endif
}

void MainWindow::restoreWindowGeometry() {
    QRect geometry = settings->windowGeometry();
    this->resize(geometry.size());
    this->move(geometry.x(), geometry.y());
}

MainWindow::~MainWindow() {
}

//#############################################################
//######################## OTHERS #############################
//#############################################################

void MainWindow::setInfoString(QString text) {
    infoOverlay->setText(text);
    setWindowTitle(text);
}

void MainWindow::slotShowControls(bool x) {
    x ? controlsOverlay->show() : controlsOverlay->hide();
}

void MainWindow::slotShowInfo(bool x) {
    x ? infoOverlay->show() : infoOverlay->hide();
}

//#############################################################
//###################### ACTION SLOTS #########################
//#############################################################

void MainWindow::switchFitMode() {
    if(fitMode == 0) {
        this->slotFitNormal();
    } else {
        this->slotFitAll();
    }
}

void MainWindow::slotFitAll() {
    fitMode = 0;
    emit signalFitAll();
}

void MainWindow::slotFitWidth() {
    fitMode = 1;
    emit signalFitWidth();
}

void MainWindow::slotFitNormal() {
    fitMode = 2;
    emit signalFitNormal();
}

void MainWindow::slotTriggerFullscreen() {
    if(fullscreen) {
        // do not save immediately on application start
        if(!this->isHidden())
            saveWindowGeometry();

        //this->hide();
        //move to target screen
        int display = settings->lastDisplay();
        if(desktopWidget->screenCount() > display &&
           display != desktopWidget->screenNumber(this))
        {
            this->move(desktopWidget->screenGeometry(display).x(),
                       desktopWidget->screenGeometry(display).y());
        }

        if(borderlessEnabled) {
            // there is some stupid bullshit with this mode under X11
            this->setWindowFlags(Qt::FramelessWindowHint);
            this->showMaximized();;
        } else {
            this->showFullScreen();
        }
        emit signalFullscreenEnabled(true);

    } else {
        this->setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
        this->showNormal();
        restoreWindowGeometry();
        this->activateWindow();
        this->raise();
        emit signalFullscreenEnabled(false);
    }
    fullscreen = fullscreen?false:true;
}

void MainWindow::slotMinimize() {
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::slotResize(QSize newSize) {
    qDebug() << "resize!" << newSize;
}

void MainWindow::slotResizeDialog() {
    ResizeDialog resizeDialog(this);
    connect(&resizeDialog, SIGNAL(sizeSelected(QSize)), this, SLOT(slotResize(QSize)));
    resizeDialog.exec();
 }

void MainWindow::slotRotateLeft() {
    core->rotateImage(-90);
}

void MainWindow::slotRotateRight() {
    core->rotateImage(90);
}

void MainWindow::slotCrop() {
    this->slotFitAll();
    imageViewer->crop();
}

void MainWindow::slotSelectWallpaper() {
    this->slotFitAll();
    imageViewer->selectWallpaper();
}

void MainWindow::slotSaveDialog() {
    const QString imagesFilter = settings->supportedFormatsString();
    QString fileName = core->getCurrentFilePath();
    fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                            fileName,
                                            imagesFilter);
    emit fileSaved(fileName);
}

void MainWindow::slotOpenDialog() {
    QFileDialog dialog;
    QStringList imageFilter;
    imageFilter.append(settings->supportedFormatsString());
    imageFilter.append("All Files (*)");
    QString lastDir = settings->lastDirectory();
    dialog.setDirectory(lastDir);
    dialog.setNameFilters(imageFilter);
    dialog.setWindowTitle("Open image");
    //dialog.setParent(this);
    dialog.setWindowModality(Qt::ApplicationModal);
    connect(&dialog, SIGNAL(fileSelected(QString)), this, SIGNAL(fileOpened(QString)));
    dialog.exec();
}

void MainWindow::showSettings() {
    SettingsDialog settingsDialog;
    settingsDialog.exec();
}
