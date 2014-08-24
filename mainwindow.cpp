#include "mainwindow.h"

MainWindow::MainWindow()
{
    init();
    modeFitAll->setChecked(true);
    setCurrentDirectory("/home/mitcher/projects/tests/");
    setMinimumSize(QSize(400,300));
    setWindowTitle(tr("qimgv 0.12"));
    resize(800, 650);
 //   setDefaultLogo();
}

void MainWindow::init()
{
    c = new Core();
    setCentralWidget(c->getMainWidget());

    createActions();
    createMenus();

    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp";
    currentDir.setNameFilters(filters);

    openFinished = true;
}

void MainWindow::setCurrentDirectory(QString path) {
    currentDir.setCurrent(path);
    currentDir.setNameFilters(filters);
    fileList = currentDir.entryList();
}



void MainWindow::setDefaultLogo()
{
    c->scrollArea->setImagePath(QString(":/images/res/logo.png"));
}

/* opens file specified by argument
 * stops current movie animation (if any)
 * loads either new pixmap or movie and places it into imgLabel
 * updates current fileInfo
 * updates window title, checkable menu actions
 * resizes imgLabel according to set fit options
 */
void MainWindow::open(QString filePath)
{
    c->scrollArea->setImagePath(filePath);

//     setCurrentDirectory(filePath);
//     updateWindowTitle();
//     updateInfoOverlay();
//     scaleFactor = 1.0;
//     updateActions();
//     fitImage();
}

/* resizes imgLabel according to set fit options,
 * which is then automatically redrawn
 */

void MainWindow::fitModeAll()
{
    if(modeFitAll->isChecked())
    {
        modeFitWidth->setChecked(false);
        modeFitNormal->setChecked(false);
    }
}

void MainWindow::fitModeWidth()
{
    bool isChecked = !modeFitWidth->isChecked();

    modeFitAll->setChecked(isChecked);
    modeFitNormal->setChecked(isChecked);
}

void MainWindow::fitModeNormal()
{
    c->scrollArea->fitImageOriginal();
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(Qt::Key_O);
    this->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), c, SLOT(openDialog()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Alt+X"));
    this->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    nextAct = new QAction(tr("N&ext"), this);
    nextAct->setShortcut(Qt::Key_Right);
    nextAct->setEnabled(true);
    this->addAction(nextAct);
    connect(nextAct, SIGNAL(triggered()), this, SLOT(next()));

    prevAct = new QAction(tr("P&rev"), this);
    prevAct->setShortcut(Qt::Key_Left);
    this->addAction(prevAct);
    connect(prevAct, SIGNAL(triggered()), this, SLOT(prev()));

    zoomInAct = new QAction(tr("Zoom &In (10%)"), this);
    zoomInAct->setShortcut(Qt::Key_Up);
    this->addAction(zoomInAct);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (10%)"), this);
    zoomOutAct->setShortcut(Qt::Key_Down);
    this->addAction(zoomOutAct);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    modeFitNormal = new QAction(tr("&Normal Size"), this);
    modeFitNormal->setShortcut(Qt::Key_N);
    modeFitNormal->setEnabled(false);
    modeFitNormal->setCheckable(true);
    this->addAction(modeFitNormal);
    connect(modeFitNormal, SIGNAL(triggered()), this, SLOT(fitModeNormal()));

    modeFitAll = new QAction(tr("Fit all"), this);
    modeFitAll->setEnabled(false);
    modeFitAll->setCheckable(true);
    modeFitAll->setShortcut(Qt::Key_A);
    this->addAction(modeFitAll);
    connect(modeFitAll, SIGNAL(triggered()), this, SLOT(fitModeAll()));

    modeFitWidth = new QAction(tr("Fit &width"), this);
    modeFitWidth->setEnabled(false);
    modeFitWidth->setCheckable(true);
    modeFitWidth->setShortcut(Qt::Key_W);
    this->addAction(modeFitWidth);
    connect(modeFitWidth, SIGNAL(triggered()), this, SLOT(fitModeWidth()));

    fullscreenEnabledAct = new QAction(tr("&Fullscreen"), this);
    fullscreenEnabledAct->setEnabled(true);
    fullscreenEnabledAct->setCheckable(true);
    fullscreenEnabledAct->setShortcut(Qt::Key_F);
    this->addAction(fullscreenEnabledAct);
    connect(fullscreenEnabledAct, SIGNAL(triggered()), this, SLOT(switchFullscreen()));

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

void MainWindow::triggerFullscreen()
{
    this->fullscreenEnabledAct->trigger();
}

void MainWindow::switchFullscreen()
{
    if(fullscreenEnabledAct->isChecked())
    {
        overlay->setHidden(false);
        cOverlay->setHidden(false);
        this->menuBar()->hide();
        this->showFullScreen();
    }
    else
    {
        overlay->setHidden(true);
        cOverlay->setHidden(true);
        this->menuBar()->show();
        this->showNormal();
    }
    updateMapOverlay();
}

/* changes current position in directory
 * loads image at that position
 */
void MainWindow::next()
{
//     if(currentDir.exists() && fileList.length()) {
//         if(++mFileInfo.fileNumber>=fileList.length()) {
//             mFileInfo.fileNumber=0;
//         }
//         QString fName = currentDir.currentPath()+"/"+fileList.at(mFileInfo.fileNumber);
//         mFileInfo.setFile(&fName);
//         open(fName);
//     }
}
/* same as above
 */
void MainWindow::prev()
{
//     if(currentDir.exists() && fileList.length()) {
//         if(--mFileInfo.fileNumber<0) {
//             mFileInfo.fileNumber=fileList.length()-1;
//         }
//         QString fName = currentDir.currentPath()+"/"+fileList.at(mFileInfo.fileNumber);
//         mFileInfo.setFile(&fName);
//         open(fName);
//     }
}

void MainWindow::zoomIn()
{
    c->scrollArea->scaleImage(1.25);
}

void MainWindow::zoomOut()
{
    c->scrollArea->scaleImage(0.9);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    //overlay->updateSize();
    //cOverlay->updateSize();
    //mOverlay->updateSize();
    //updateMapOverlay();
}

/* calculates difference between current viewing area and total image size
 * hides overlay if image fits in view entirely
 * calculates image aspect ratio
 * calculates current view area position in percent
 * calls updateMap with above values
 */
void MainWindow::updateMapOverlay()
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
        mOverlay->hide();
    }
    else
    {
        mOverlay->show();
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
        mOverlay->updateMap(widthDifferenceRatio, heightDifferenceRatio, viewportPositionX, viewportPositionY, imageAspectRatio);
    }
}

void MainWindow::updateWindowTitle()
{
//     setWindowTitle(mFileInfo.getInfo()+"[ "+QString::number(mFileInfo.fileNumber+1)+" / "+QString::number(fileList.length())+" ] - qimgv");
}

void MainWindow::updateInfoOverlay()
{
//     overlay->setText(mFileInfo.getInfo()+"[ "+QString::number(mFileInfo.fileNumber+1)+" / "+QString::number(fileList.length())+" ]");
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    event->angleDelta().ry() > 0 ? c->showPrevImage() : c->showNextImage();
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

void MainWindow::minimizeWindow()
{
    this->setWindowState(Qt::WindowMinimized);
}

MainWindow::~MainWindow()
{

}
