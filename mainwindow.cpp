#include "mainwindow.h"

MainWindow::MainWindow() :
    imageViewer(NULL),
    videoPlayer(NULL),
    settingsDialog(NULL),
    panel(NULL),
    currentViewer(0),
    layout(NULL),
    borderlessEnabled(false)
{
    resize(1100, 700);
    setMinimumSize(QSize(400,300));
    this->setMouseTracking(true);
    init();
    readSettingsInitial();
    setWindowTitle(QCoreApplication::applicationName() +
                   " " +
                   QCoreApplication::applicationVersion());
    setWindowIcon(QIcon(":/images/res/pepper32.png"));
}

void MainWindow::init() {
    settingsDialog = new SettingsDialog();
    imageViewer = new ImageViewer(this);
    imageViewer->hide();
    videoPlayer = new VideoPlayer(this);
    videoPlayer->hide();

    QWidget* central = new QWidget();
    controlsOverlay = new ControlsOverlay(imageViewer);
    controlsOverlay->hide();
    infoOverlay = new textOverlay(imageViewer);

    layout = new QVBoxLayout;
    //central->setAttribute(Qt::WA_TransparentForMouseEvents);
    central->setAttribute(Qt::WA_MouseTracking);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    central->setSizePolicy(sizePolicy);
    layout->setContentsMargins(0, 0, 0, 0);
    central->setLayout(layout);
    this->setCentralWidget(central);

    panel = new ThumbnailStrip(this);
    panel->parentResized(size());

    core = new Core();

    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));

    enableImageViewer();

    connect(this, SIGNAL(resized(QSize)), panel, SLOT(parentResized(QSize)));

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

    // when loaded
    connect(core, SIGNAL(imageChanged(int)),
            panel, SLOT(selectThumbnail(int)));

    connect(this, SIGNAL(fileSaved(QString)),
            core, SLOT(saveImage(QString)));

    connect(panel, SIGNAL(thumbnailClicked(int)),
            core, SLOT(loadImageByPos(int)));

    connect(panel, SIGNAL(thumbnailRequested(int)),
            core, SIGNAL(thumbnailRequested(int)));

    connect(core, SIGNAL(thumbnailReady(int, Thumbnail*)),
            panel, SLOT(setThumbnail(int, Thumbnail*)));

    connect(core, SIGNAL(cacheInitialized(int)),
            panel, SLOT(fillPanel(int)), Qt::DirectConnection);

    connect(core, SIGNAL(videoChanged(QString)),
            this, SLOT(openVideo(QString)), Qt::UniqueConnection);

    connect(videoPlayer, SIGNAL(sendDoubleClick()),
            this, SLOT(slotTriggerFullscreen()), Qt::UniqueConnection);

    core->init();

    createActions();
    createMenus();
}

void MainWindow::enableImageViewer() {
    if( currentViewer != 1 ) {
        disableVideoPlayer();
        controlsOverlay->setParent(imageViewer);
        infoOverlay->setParent(imageViewer);
        layout->addWidget(imageViewer);

        imageViewer->show();

        connect(imageViewer, SIGNAL(scalingRequested(QSize)),
                core, SLOT(rescaleForZoom(QSize)), Qt::UniqueConnection);

        connect(core, SIGNAL(scalingFinished(QPixmap*)),
                imageViewer, SLOT(updateImage(QPixmap*)), Qt::UniqueConnection);

        connect(core, SIGNAL(frameChanged(QPixmap*)),
                imageViewer, SLOT(updateImage(QPixmap*)),
                static_cast<Qt::ConnectionType>(Qt::DirectConnection | Qt::UniqueConnection));
        // reload after image edits
        connect(core, SIGNAL(imageAltered(QPixmap*)),
                imageViewer, SLOT(displayImage(QPixmap*)), Qt::UniqueConnection);

        connect(imageViewer, SIGNAL(cropSelected(QRect)),
                core, SLOT(crop(QRect)), Qt::UniqueConnection);

        connect(imageViewer, SIGNAL(wallpaperSelected(QRect)),
                core, SLOT(setWallpaper(QRect)), Qt::UniqueConnection);

        connect(core, SIGNAL(signalSetImage(QPixmap*)),
                this, SLOT(openImage(QPixmap*)), Qt::UniqueConnection);

        connect(this, SIGNAL(signalFitAll()),
                imageViewer, SLOT(slotFitAll()), Qt::UniqueConnection);

        connect(this, SIGNAL(signalFitWidth()),
                imageViewer, SLOT(slotFitWidth()), Qt::UniqueConnection);

        connect(this, SIGNAL(signalFitNormal()),
                imageViewer, SLOT(slotFitNormal()), Qt::UniqueConnection);

        connect(imageViewer, SIGNAL(sendRightDoubleClick()),
                this, SLOT(switchFitMode()), Qt::UniqueConnection);

        connect(this, SIGNAL(signalZoomIn()),
                imageViewer, SLOT(slotZoomIn()), Qt::UniqueConnection);

        connect(this, SIGNAL(signalZoomOut()),
                imageViewer, SLOT(slotZoomOut()), Qt::UniqueConnection);

        connect(imageViewer, SIGNAL(sendDoubleClick()),
                this, SLOT(slotTriggerFullscreen()), Qt::UniqueConnection);

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

    disconnect(core, SIGNAL(signalSetImage(QPixmap*)),
            imageViewer, SLOT(displayImage(QPixmap*)));

    disconnect(imageViewer, SIGNAL(sendRightDoubleClick()),
            this, SLOT(switchFitMode()));

    disconnect(this, SIGNAL(signalZoomIn()),
            imageViewer, SLOT(slotZoomIn()));

    disconnect(this, SIGNAL(signalZoomOut()),
            imageViewer, SLOT(slotZoomOut()));

    disconnect(imageViewer, SIGNAL(sendDoubleClick()),
            this, SLOT(slotTriggerFullscreen()));

    currentViewer = 0;
    imageViewer->hide();
}

void MainWindow::enableVideoPlayer() {
    if( currentViewer != 2 ) {
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

void MainWindow::openVideo(QString path) {
    enableVideoPlayer();
    videoPlayer->play(path);
}

void MainWindow::openImage(QPixmap *pixmap) {
    enableImageViewer();
    imageViewer->displayImage(pixmap);
}

void MainWindow::open(QString path) {
    core->loadImageBlocking(path);
}

void MainWindow::readSettingsInitial() {
    readSettings();
    if ( !globalSettings->fullscreenMode() ) {
        restoreWindowGeometry();
    }
}

void MainWindow::readSettings() {
    borderlessEnabled = globalSettings->fullscreenTaskbarShown();
    menuBar()->setHidden( globalSettings->menuBarHidden() );
    panelPosition = globalSettings->panelPosition();
    int fitMode = globalSettings->imageFitMode();
    if(fitMode == 1) {
        slotFitWidth();
    }
    else if(fitMode == 2) {
        slotFitNormal();
    }
    else {
        slotFitAll();
    }
    calculatePanelTriggerArea();
    emit resized(size());
}

void MainWindow::slotOpenDialog() {
    QFileDialog dialog;
    QStringList imageFilter;
    imageFilter.append(globalSettings->supportedFormatsString());
    imageFilter.append("All Files (*)");
    QString lastDir = globalSettings->lastDirectory();
    dialog.setDirectory(lastDir);
    dialog.setNameFilters(imageFilter);
    dialog.setWindowTitle("Open image");
    //dialog.setParent(this);
    dialog.setWindowModality(Qt::ApplicationModal);
    connect(&dialog, SIGNAL(fileSelected(QString)), this, SIGNAL(fileOpened(QString)));
    dialog.exec();
}

void MainWindow::slotShowControls(bool x) {
    x?controlsOverlay->show():controlsOverlay->hide();
}

void MainWindow::slotShowInfo(bool x) {
    x?infoOverlay->show():infoOverlay->hide();
}

void MainWindow::setInfoString(QString text) {
    infoOverlay->setText(text);
    setWindowTitle(text);
}

void MainWindow::slotFitAll()
{
    modeFitWidth->setChecked(false);
    modeFitNormal->setChecked(false);
    modeFitAll->setChecked(true);
    emit signalFitAll();
}

void MainWindow::slotFitWidth()
{
    modeFitAll->setChecked(false);
    modeFitNormal->setChecked(false);
    modeFitWidth->setChecked(true);
    emit signalFitWidth();
}

void MainWindow::slotFitNormal()
{
    modeFitAll->setChecked(false);
    modeFitWidth->setChecked(false);
    modeFitNormal->setChecked(true);
    emit signalFitNormal();
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(Qt::CTRL+Qt::Key_O);
    this->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(slotOpenDialog()));

    saveAct = new QAction(tr("Save"), this);
    saveAct->setShortcut(Qt::CTRL+Qt::Key_S);
    this->addAction(saveAct);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(slotSaveDialog()));

    settingsAct = new QAction(tr("&Preferences"), this);
    settingsAct->setShortcut(Qt::CTRL+Qt::Key_P);
    this->addAction(settingsAct);
    connect(settingsAct, SIGNAL(triggered()), settingsDialog, SLOT(show()));

    QList<QKeySequence> exitShortcuts;
    exitShortcuts << Qt::ALT+Qt::Key_X << Qt::CTRL+Qt::Key_Q;
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(exitShortcuts);
    this->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    rotateLeftAct = new QAction(tr("Rotate L&eft"), this);
    rotateLeftAct->setShortcut(Qt::Key_L);
    this->addAction(rotateLeftAct);
    connect(rotateLeftAct, SIGNAL(triggered()), this, SLOT(slotRotateLeft()));

    cropAct = new QAction(tr("C&rop"), this);
    cropAct->setShortcut(Qt::Key_X);
    this->addAction(cropAct);
    connect(cropAct, &QAction::triggered, [=]() {
        this->slotFitAll();
        imageViewer->crop();
    });

    selectWallpaperAct = new QAction(tr("Set wallpaper"), this);
    selectWallpaperAct->setShortcut(Qt::Key_Z);
    this->addAction(selectWallpaperAct);
    connect(selectWallpaperAct, &QAction::triggered, [=]() {
        this->slotFitAll();
        imageViewer->selectWallpaper();
    });

    rotateRightAct = new QAction(tr("Rotate R&ight"), this);
    rotateRightAct->setShortcut(Qt::Key_R);
    this->addAction(rotateRightAct);
    connect(rotateRightAct, SIGNAL(triggered()), this, SLOT(slotRotateRight()));

    QList<QKeySequence> nextShortcuts;
    nextShortcuts << Qt::Key_Right << Qt::Key_D;
    nextAct = new QAction(tr("N&ext"), this);
    nextAct->setShortcuts(nextShortcuts);
    nextAct->setEnabled(true);
    this->addAction(nextAct);
    connect(nextAct, SIGNAL(triggered()), core, SLOT(slotNextImage()));

    QList<QKeySequence> prevShortcuts;
    prevShortcuts << Qt::Key_Left << Qt::Key_A;
    prevAct = new QAction(tr("P&rev"), this);
    prevAct->setShortcuts(prevShortcuts);
    this->addAction(prevAct);
    connect(prevAct, SIGNAL(triggered()), core, SLOT(slotPrevImage()));

    QList<QKeySequence> zoomInShortcuts;
    zoomInShortcuts << Qt::Key_Up << Qt::Key_W;
    zoomInAct = new QAction(tr("Zoom &In (10%)"), this);
    zoomInAct->setShortcuts(zoomInShortcuts);
    this->addAction(zoomInAct);
    connect(zoomInAct, SIGNAL(triggered()), imageViewer, SLOT(slotZoomIn()));

    QList<QKeySequence> zoomOutShortcuts;
    zoomOutShortcuts << Qt::Key_Down << Qt::Key_S;
    zoomOutAct = new QAction(tr("Zoom &Out (10%)"), this);
    zoomOutAct->setShortcuts(zoomOutShortcuts);
    this->addAction(zoomOutAct);
    connect(zoomOutAct, SIGNAL(triggered()), imageViewer, SLOT(slotZoomOut()));

    modeFitNormal = new QAction(tr("&Normal Size"), this);
    modeFitNormal->setShortcut(Qt::CTRL+Qt::Key_N);
    modeFitNormal->setEnabled(false);
    modeFitNormal->setCheckable(true);
    this->addAction(modeFitNormal);
    connect(modeFitNormal, SIGNAL(triggered()), this, SLOT(slotFitNormal()));

    modeFitAll = new QAction(tr("Fit all"), this);
    modeFitAll->setEnabled(false);
    modeFitAll->setCheckable(true);
    modeFitAll->setShortcut(Qt::CTRL+Qt::Key_A);
    this->addAction(modeFitAll);
    connect(modeFitAll, SIGNAL(triggered()), this, SLOT(slotFitAll()));

    modeFitWidth = new QAction(tr("Fit &width"), this);
    modeFitWidth->setEnabled(false);
    modeFitWidth->setCheckable(true);
    modeFitWidth->setShortcut(Qt::CTRL+Qt::Key_W);
    this->addAction(modeFitWidth);
    connect(modeFitWidth, SIGNAL(triggered()), this, SLOT(slotFitWidth()));

    fullscreenEnabledAct = new QAction(tr("&Fullscreen"), this);
    fullscreenEnabledAct->setEnabled(true);
    fullscreenEnabledAct->setCheckable(true);
    fullscreenEnabledAct->setShortcut(Qt::Key_F);
    this->addAction(fullscreenEnabledAct);
    connect(fullscreenEnabledAct, SIGNAL(triggered()),
            this, SLOT(slotFullscreen()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(slotAbout()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    modeFitNormal->setEnabled(true);
    modeFitAll->setEnabled(true);
    modeFitWidth->setEnabled(true);
}

void MainWindow::createMenus() {
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(settingsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(cropAct);
    editMenu->addSeparator();
    editMenu->addAction(rotateLeftAct);
    editMenu->addAction(rotateRightAct);
    editMenu->addSeparator();
    editMenu->addAction(selectWallpaperAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(fullscreenEnabledAct);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addSeparator();
    viewMenu->addAction(modeFitNormal);
    viewMenu->addAction(modeFitAll);
    viewMenu->addAction(modeFitWidth);

    navigationMenu = new QMenu(tr("&Navigate"), this);
    navigationMenu->addAction(nextAct);
    navigationMenu->addAction(prevAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(navigationMenu);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::slotTriggerFullscreen() {
    this->fullscreenEnabledAct->trigger();
}

void MainWindow::slotFullscreen() {
    if(fullscreenEnabledAct->isChecked()) {
        saveWindowGeometry();
        this->menuBar()->hide();
        if(borderlessEnabled) {
            this->setWindowFlags(Qt::FramelessWindowHint);
            this->showMaximized();
        } else {
            this->showFullScreen();
        }
        emit signalFullscreenEnabled(true);
    }
    else {
        showMenuBar();
        this->setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
        this->show();
        restoreWindowGeometry();
        this->activateWindow();
        this->raise();
        emit signalFullscreenEnabled(false);
    }
}

void MainWindow::slotMinimize() {
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::switchFitMode() {
    if(modeFitAll->isChecked()) {
        this->slotFitNormal();
    }
    else {
        this->slotFitAll();
    }
}

void MainWindow::slotSetInfoString(QString info) {
    infoOverlay->setText(info);
    info.append(" - ");
    info.append(QCoreApplication::applicationName());
    setWindowTitle(info);
}

void MainWindow::slotRotateLeft() {
    core->rotateImage(-90);
}

void MainWindow::slotRotateRight() {
    core->rotateImage(90);
}

void MainWindow::slotSaveDialog() {
    const QString imagesFilter = globalSettings->supportedFormatsString();
    QString fileName = core->getCurrentFilePath();
    fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               fileName,
                               imagesFilter);
    emit fileSaved(fileName);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    Q_UNUSED(event)
    if(panel) {
        calculatePanelTriggerArea();
        emit resized(size());
    }
    updateOverlays();
}

void MainWindow::calculatePanelTriggerArea() {
    switch (panelPosition) {
        case LEFT:   panelArea.setRect(0, 0, 80, height());
                     break;
        case RIGHT:  panelArea.setRect(width() - 80, 0, width(), height());
                     break;
        case BOTTOM: panelArea.setRect(0, height() - 80, width() - 180, height());
                     break;
        case TOP:    panelArea.setRect(0, 0, width(), 80);
                     if(isFullScreen()) {
                        panelArea.setRight(width() - 250);
                     }
                     break;
    }
}

void MainWindow::updateOverlays() {
    controlsOverlay->updateSize(this->centralWidget()->size());
    infoOverlay->updateWidth(this->centralWidget()->width());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    event->accept();
    saveWindowGeometry();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if(event->buttons() != Qt::RightButton && event->buttons() != Qt::LeftButton) {
        if(panelArea.contains(event->pos()) && panel) {
            panel->show();
        }
        event->ignore();
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    if(!panel->isHidden() && panel->rect().contains(event->pos())) {
        event->ignore();
        return;
    }
    event->accept();
    if(event->angleDelta().ry() < 0) {
        emit signalNextImage();
    }
    else {
        emit signalPrevImage();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QMainWindow::keyPressEvent(event);
    if (event->key() == Qt::Key_Space)
    {
        switchFitMode();
    }
    if(event->key() == Qt::Key_M) {
        triggerMenuBar();
    }
}

void MainWindow::showMenuBar() {
    if( !globalSettings->menuBarHidden() ) {
        menuBar()->show();
    }
}

void MainWindow::triggerMenuBar() {
    if(this->menuBar()->isHidden()) {
        this->menuBar()->show();
    } else {
        this->menuBar()->hide();
    }
    globalSettings->setMenuBarHidden(this->menuBar()->isHidden());
}

bool MainWindow::eventFilter(QObject *target, QEvent *event) {
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::slotAbout() {
    QMessageBox msgBox;
    QSpacerItem* horizontalSpacer =
            new QSpacerItem(250,
                            0,
                            QSizePolicy::Minimum,
                            QSizePolicy::Expanding);
    msgBox.setWindowTitle("About " +
                          QCoreApplication::applicationName() +
                          " " +
                          QCoreApplication::applicationVersion());
    QString message;
    message = "A simple qt image viewer \n \nMain developer: \n    Easymodo";
    message.append("\nDeveloper:\n    Sevoan");
    message.append("\n\n This is a pre-release software.");
    message.append("\n Expect bugs.");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer,
                    layout->rowCount(),
                    0,
                    1,
                    layout->columnCount());
    msgBox.exec();
}

void MainWindow::close() {
    if (QThreadPool::globalInstance()->activeThreadCount()) {
        this->setWindowTitle("closing...");
        QThreadPool::globalInstance()->waitForDone();
    }
    QMainWindow::close();
}

void MainWindow::saveWindowGeometry() {
    globalSettings->setWindowGeometry(this->saveGeometry());
}

void MainWindow::restoreWindowGeometry() {
    this->restoreGeometry(globalSettings->windowGeometry());
}

MainWindow::~MainWindow() {

}
