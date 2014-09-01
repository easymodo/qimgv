#include "mainwindow.h"

MainWindow::MainWindow()
{
    init();
    modeFitAll->setChecked(true);
    setMinimumSize(QSize(400,300));
    setWindowTitle(tr("qimgv 0.1999"));
    resize(800, 650);
}

void MainWindow::init() {
    createActions();
    createMenus();
}

void MainWindow::slotFitAll()
{
    if(modeFitAll->isChecked())
    {
        modeFitWidth->setChecked(false);
        modeFitNormal->setChecked(false);
        emit signalFitAll();
    }
}

void MainWindow::slotFitWidth()
{
    if(modeFitWidth->isChecked()) {
        modeFitAll->setChecked(false);
        modeFitNormal->setChecked(false);
        emit signalFitWidth();
    }
}

void MainWindow::slotFitNormal()
{
    if(modeFitNormal->isChecked()) {
        modeFitAll->setChecked(false);
        modeFitWidth->setChecked(false);
        emit signalFitNormal();
    }

}

void MainWindow::slotOpenDialog() {
    emit signalOpenDialog();
}

void MainWindow::slotNextImage() {
    emit signalNextImage();
}

void MainWindow::slotPrevImage() {
    emit signalPrevImage();
}

void MainWindow::slotZoomIn() {
    emit signalZoomOut();
}

void MainWindow::slotZoomOut() {
    emit signalZoomOut();
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(Qt::Key_O);
    this->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(slotOpenDialog()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Alt+X"));
    this->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    nextAct = new QAction(tr("N&ext"), this);
    nextAct->setShortcut(Qt::Key_Right);
    nextAct->setEnabled(true);
    this->addAction(nextAct);
    connect(nextAct, SIGNAL(triggered()), this, SLOT(slotNextImage()));

    prevAct = new QAction(tr("P&rev"), this);
    prevAct->setShortcut(Qt::Key_Left);
    this->addAction(prevAct);
    connect(prevAct, SIGNAL(triggered()), this, SLOT(slotPrevImage()));

    zoomInAct = new QAction(tr("Zoom &In (10%)"), this);
    zoomInAct->setShortcut(Qt::Key_Up);
    this->addAction(zoomInAct);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(slotZoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (10%)"), this);
    zoomOutAct->setShortcut(Qt::Key_Down);
    this->addAction(zoomOutAct);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(slotZoomOut()));

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
    connect(fullscreenEnabledAct, SIGNAL(triggered()), this, SLOT(slotFullscreen()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    modeFitNormal->setEnabled(true);
    modeFitAll->setEnabled(true);
    modeFitWidth->setEnabled(true);
}

void MainWindow::updateActions()
{
//     if(mCurrentImage->getType() != Type::STATIC)
//     {
//         zoomInAct->setEnabled(false);
//         zoomOutAct->setEnabled(false);
//     }
//     else {
//         zoomInAct->setEnabled(true);
//         zoomOutAct->setEnabled(true);
//     }
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
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
    }
    else
    {
        this->menuBar()->show();
        this->showNormal();
    }
}

void MainWindow::slotMinimize()
{
    this->setWindowState(Qt::WindowMinimized);
}

/*void MainWindow::updateMapOverlay()
{
    double widthDifferenceRatio = 1.0;
    double heightDifferenceRatio = 1.0;
    QSize imageSize = mScrollArea->size();
    QSize viewportSize = mScrollArea->viewport()->size();

    if (imageSize.width()>viewportSize.width())
    {
        widthDifferenceRatio=(double)viewportSize.width()/imageSize.width();
    }
    if (imageSize.height()>viewportSize.height())
    {
        heightDifferenceRatio=(double)viewportSize.height()/imageSize.height();
    }
    if (widthDifferenceRatio>=1 && heightDifferenceRatio>=1)
    {
        //mOverlay->hide();
    }
    else
    {
        //mOverlay->show();
        double viewportPositionX;
        double viewportPositionY;
        double imageAspectRatio = (double)imageSize.height() / imageSize.width();
        if (mScrollArea->horizontalScrollBar()->maximum() == 0)
        {
            viewportPositionX=0.0;
        }
        else
        {
            viewportPositionX=(double)mScrollArea->horizontalScrollBar()->value()/mScrollArea->horizontalScrollBar()->maximum();
        }
        if (mScrollArea->verticalScrollBar()->maximum() == 0)
        {
            viewportPositionY=0.0;
        }
        else
        {
            viewportPositionY=(double)mScrollArea->verticalScrollBar()->value()/mScrollArea->verticalScrollBar()->maximum();
        }
        //mOverlay->updateMap(widthDifferenceRatio, heightDifferenceRatio, viewportPositionX, viewportPositionY, imageAspectRatio);
    }
}
*/

void MainWindow::wheelEvent(QWheelEvent *event)
{
    event->angleDelta().ry() < 0 ? slotNextImage() : slotPrevImage();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
    if (event->key() == Qt::Key_Space)
    {
        //switchFitMode();
    }
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    QMainWindow::eventFilter(target, event);
}


MainWindow::~MainWindow()
{

}
