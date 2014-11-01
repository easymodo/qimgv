#include "mainwindow.h"

MainWindow::MainWindow() :
    imageViewer(NULL),
    settingsDialog(NULL)
{
    init();
    resize(800, 650);
    readSettings();
    setMinimumSize(QSize(400,300));
    setWindowTitle(QCoreApplication::applicationName() +
                   " " +
                   QCoreApplication::applicationVersion());
    setWindowIcon(QIcon(":/images/res/pepper32.png"));
}

void MainWindow::init() {
    settingsDialog = new SettingsDialog();
    imageViewer = new ImageViewer(this);
    controlsOverlay = new ControlsOverlay(imageViewer);
    infoOverlay = new textOverlay(imageViewer, TOP);
    //messageOverlay = new textOverlay(imageViewer, BOTTOM);
    //messageOverlay->setText("Loading...");
    infoOverlay->hide();
    //messageOverlay->hide();
    controlsOverlay->hide();
    this->setCentralWidget(imageViewer);
    core = new Core();

    createActions();
    createMenus();

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

   // connect(imageViewer, SIGNAL(imageChanged()),
   //         messageOverlay, SLOT(hide()));

    connect(this, SIGNAL(signalFullscreenEnabled(bool)),
            this, SLOT(slotShowControls(bool)));

    connect(this, SIGNAL(signalFullscreenEnabled(bool)),
            this, SLOT(slotShowInfo(bool)));

    connect(core, SIGNAL(infoStringChanged(QString)),
            this, SLOT(setInfoString(QString)));

    connect(core, SIGNAL(signalUnsetImage()),
            imageViewer, SLOT(freeImage()));

    connect(core, SIGNAL(signalSetImage(Image*)),
            imageViewer, SLOT(displayImage(Image*)));

    connect(controlsOverlay, SIGNAL(exitClicked()),
            this, SLOT(close()));

    connect(controlsOverlay, SIGNAL(exitFullscreenClicked()),
            this, SLOT(slotTriggerFullscreen()));

    connect(controlsOverlay, SIGNAL(minimizeClicked()),
            this, SLOT(slotMinimize()));
}

void MainWindow::open(QString path) {
    core->loadImage(path);
}

void MainWindow::readSettings() {
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

    settingsAct = new QAction(tr("&Settings"), this);
    settingsAct->setShortcut(Qt::Key_S);
    this->addAction(settingsAct);
    connect(settingsAct, SIGNAL(triggered()), settingsDialog, SLOT(show()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Alt+X"));
    this->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

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
    fileMenu->addSeparator();
    fileMenu->addAction(settingsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

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
        this->menuBar()->show();
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

void MainWindow::resizeEvent(QResizeEvent* event) {
    controlsOverlay->updateSize();
    infoOverlay->updateWidth();
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
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    QMainWindow::eventFilter(target, event);
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
    msgBox.setText("A simple qt image viewer \n \nMain developer: \nEugene G.");
    msgBox.setIcon(QMessageBox::Information);
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer,
                    layout->rowCount(),
                    0,
                    1,
                    layout->columnCount());
    msgBox.exec();
}

MainWindow::~MainWindow()
{

}
