#include "mainwindow.h"

MainWindow::MainWindow() :
    imageViewer(NULL),
    settingsDialog(NULL),
    panel(NULL)
{
    resize(800, 600);
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

    controlsOverlay = new ControlsOverlay(imageViewer);
    controlsOverlay->hide();

    infoOverlay = new textOverlay(imageViewer);

    this->setCentralWidget(imageViewer);

    panel = new ThumbnailStrip(this);
    panel->parentResized(size());

    core = new Core();

    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));

    connect(this, SIGNAL(signalNextImage()),
            core, SLOT(slotNextImage()));

    connect(this, SIGNAL(signalPrevImage()),
            core, SLOT(slotPrevImage()));

    connect(this, SIGNAL(fileOpened(QString)),
            core, SLOT(loadImage(QString)));

    connect(this, SIGNAL(signalFitAll()),
            imageViewer, SLOT(slotFitAll()));

    connect(this, SIGNAL(signalFitWidth()),
            imageViewer, SLOT(slotFitWidth()));

    connect(this, SIGNAL(signalFitNormal()),
            imageViewer, SLOT(slotFitNormal()));

    connect(imageViewer, SIGNAL(sendRightDoubleClick()),
            this, SLOT(switchFitMode()));

    connect(this, SIGNAL(signalZoomIn()),
            imageViewer, SLOT(slotZoomIn()));

    connect(this, SIGNAL(signalZoomOut()),
            imageViewer, SLOT(slotZoomOut()));

    connect(imageViewer, SIGNAL(sendDoubleClick()),
            this, SLOT(slotTriggerFullscreen()));

    connect(this, SIGNAL(signalFullscreenEnabled(bool)),
            this, SLOT(slotShowControls(bool)));

    connect(this, SIGNAL(signalFullscreenEnabled(bool)),
            this, SLOT(slotShowInfo(bool)));

    connect(core, SIGNAL(infoStringChanged(QString)),
            this, SLOT(setInfoString(QString)));

    connect(core, SIGNAL(signalSetImage(QPixmap*)),
            imageViewer, SLOT(displayImage(QPixmap*)));

    connect(controlsOverlay, SIGNAL(exitClicked()),
            this, SLOT(close()));

    connect(controlsOverlay, SIGNAL(exitFullscreenClicked()),
            this, SLOT(slotTriggerFullscreen()));

    connect(controlsOverlay, SIGNAL(minimizeClicked()),
            this, SLOT(slotMinimize()));

    connect(imageViewer, SIGNAL(cropSelected(QRect)),
            core, SLOT(crop(QRect)));

    // reload after image edits
    connect(core, SIGNAL(imageAltered(QPixmap*)),
            imageViewer, SLOT(displayImage(QPixmap*)));

    // when loaded
    connect(core, SIGNAL(imageChanged(int)),
            panel, SLOT(selectThumbnail(int)));

    connect(this, SIGNAL(fileSaved(QString)),
            core, SLOT(saveImage(QString)));

    connect(panel, SIGNAL(thumbnailClicked(int)),
            core, SLOT(loadImageByPos(int)));

    connect(panel, SIGNAL(thumbnailRequested(int)),
            core, SIGNAL(thumbnailRequested(int)));

    connect(core, SIGNAL(thumbnailReady(int, const QPixmap*)),
            panel, SLOT(setThumbnail(int, const QPixmap*)));

    connect(core, SIGNAL(cacheInitialized(int)),
            panel, SLOT(fillPanel(int)), Qt::DirectConnection);

    connect(imageViewer, SIGNAL(scalingRequested(QSize)),
            core, SLOT(rescaleForZoom(QSize)));

    connect(core, SIGNAL(scalingFinished(QPixmap*)),
            imageViewer, SLOT(updateImage(QPixmap*)));

    connect(core, SIGNAL(frameChanged(QPixmap*)),
            imageViewer, SLOT(updateImage(QPixmap*)), Qt::DirectConnection);

    core->init();

    createActions();
    createMenus();
}

void MainWindow::open(QString path) {
    core->loadImageBlocking(path);
}

void MainWindow::readSettingsInitial() {
    readSettings();
    if(!globalSettings->s.value("openInFullscreen", "false").toBool()) {
        restoreWindowGeometry();
    }
}

void MainWindow::readSettings() {
    menuBar()->setHidden(globalSettings->s.value("hideMenuBar", "false").toBool());
    QString fitMode =
            globalSettings->s.value("defaultFitMode", "ALL").toString();
    if(fitMode == "WIDTH") {
        slotFitWidth();
    }
    else if(fitMode == "NORMAL") {
        slotFitNormal();
    }
    else {
        slotFitAll();
    }
}

void MainWindow::slotOpenDialog() {
    QFileDialog dialog;
    const QString imagesFilter = tr("Images (*.png *.jpg *jpeg *bmp *gif)");
    QString lastDir = globalSettings->s.value("lastDir",".").toString();
    QString str = dialog.getOpenFileName(this,
                                         tr("Open image"),
                                         lastDir,
                                         imagesFilter,
                                         0);
    emit fileOpened(str);
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
    openAct->setShortcut(Qt::Key_O);
    this->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(slotOpenDialog()));

    saveAct = new QAction(tr("Save"), this);
    saveAct->setShortcut(Qt::CTRL+Qt::Key_S);
    this->addAction(saveAct);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(slotSaveDialog()));

    settingsAct = new QAction(tr("&Settings"), this);
    settingsAct->setShortcut(Qt::Key_S);
    this->addAction(settingsAct);
    connect(settingsAct, SIGNAL(triggered()), settingsDialog, SLOT(show()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Alt+X"));
    this->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    rotateLeftAct = new QAction(tr("Rotate L&eft"), this);
    rotateLeftAct->setShortcut(tr("L"));
    this->addAction(rotateLeftAct);
    connect(rotateLeftAct, SIGNAL(triggered()), this, SLOT(slotRotateLeft()));

    cropAct = new QAction(tr("C&rop"), this);
    cropAct->setShortcut(tr("C"));
    this->addAction(cropAct);
    connect(cropAct, &QAction::triggered, [=]() {
        this->slotFitAll();
        imageViewer->crop();
    });
    rotateRightAct = new QAction(tr("Rotate R&ight"), this);
    rotateRightAct->setShortcut(tr("R"));
    this->addAction(rotateRightAct);
    connect(rotateRightAct, SIGNAL(triggered()), this, SLOT(slotRotateRight()));

    nextAct = new QAction(tr("N&ext"), this);
    nextAct->setShortcut(Qt::Key_Right);
    nextAct->setEnabled(true);
    this->addAction(nextAct);
    connect(nextAct, SIGNAL(triggered()), core, SLOT(slotNextImage()));

    prevAct = new QAction(tr("P&rev"), this);
    prevAct->setShortcut(Qt::Key_Left);
    this->addAction(prevAct);
    connect(prevAct, SIGNAL(triggered()), core, SLOT(slotPrevImage()));

    zoomInAct = new QAction(tr("Zoom &In (10%)"), this);
    zoomInAct->setShortcut(Qt::Key_Up);
    this->addAction(zoomInAct);
    connect(zoomInAct, SIGNAL(triggered()), imageViewer, SLOT(slotZoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (10%)"), this);
    zoomOutAct->setShortcut(Qt::Key_Down);
    this->addAction(zoomOutAct);
    connect(zoomOutAct, SIGNAL(triggered()), imageViewer, SLOT(slotZoomOut()));

    modeFitNormal = new QAction(tr("&Normal Size"), this);
    modeFitNormal->setShortcut(Qt::Key_N);
    modeFitNormal->setEnabled(false);
    modeFitNormal->setCheckable(true);
    this->addAction(modeFitNormal);
    connect(modeFitNormal, SIGNAL(triggered()), this, SLOT(slotFitNormal()));

    modeFitAll = new QAction(tr("Fit all"), this);
    modeFitAll->setEnabled(false);
    modeFitAll->setCheckable(true);
    modeFitAll->setShortcut(Qt::Key_A);
    this->addAction(modeFitAll);
    connect(modeFitAll, SIGNAL(triggered()), this, SLOT(slotFitAll()));

    modeFitWidth = new QAction(tr("Fit &width"), this);
    modeFitWidth->setEnabled(false);
    modeFitWidth->setCheckable(true);
    modeFitWidth->setShortcut(Qt::Key_W);
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

void MainWindow::createMenus()
{
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
    if(fullscreenEnabledAct->isChecked())
    {
        this->menuBar()->hide();
        this->showFullScreen();
        emit signalFullscreenEnabled(true);
    }
    else
    {
        showMenuBar();
        this->showNormal();
        emit signalFullscreenEnabled(false);
    }
}

void MainWindow::slotMinimize()
{
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
    const QString imagesFilter = tr("Images (*.png *.jpg *jpeg *bmp *gif)");
    QString lastDir = globalSettings->s.value("lastDir",".").toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               lastDir,
                               imagesFilter);
    emit fileSaved(fileName);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    Q_UNUSED(event)

    if(panel) {
        panel->parentResized(size());
    }
    controlsOverlay->updateSize();
    infoOverlay->updateWidth();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(!globalSettings->s.value("openInFullscreen", "false").toBool() &&
       !this->isFullScreen()) {
        saveWindowGeometry();
}
    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if(event->buttons() != Qt::RightButton && event->buttons() != Qt::LeftButton) {
        if(event->pos().y() > height()-80 && event->pos().x() < width()-130 && panel) {
            panel->show();
        }
    }
    event->ignore();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().ry() < 0) {
        emit signalNextImage();
    }
    else {
        emit signalPrevImage();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
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
    if(!globalSettings->s.value("hideMenuBar", "false").toBool()) {
        menuBar()->show();
    }
}

void MainWindow::triggerMenuBar() {
    if(this->menuBar()->isHidden()) {
        this->menuBar()->show();
    } else {
        this->menuBar()->hide();
    }
    globalSettings->s.setValue("hideMenuBar", this->menuBar()->isHidden());
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
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
    globalSettings->s.setValue("windowGeometry", this->saveGeometry());
}

void MainWindow::restoreWindowGeometry() {
    this->restoreGeometry(globalSettings->
                          s.value("windowGeometry").toByteArray());
}

MainWindow::~MainWindow()
{

}
