#include <QtWidgets>
#include "mainwindow.h"

MainWindow::MainWindow()
{
    init();
    fitModeAllAct->setChecked(true);
    //changeCurrentDir("/home/mitcher/Pictures/");
    changeCurrentDir("K://_code/sample images/");
    setWindowTitle(tr("qimgv 0.1.1"));
    resize(800, 650);
    fInfo.getInfo();
}

void MainWindow::init() {
    imgLabel = new zzLabel();
    bgColor = QColor(0,0,0,255);
    QPalette bg(bgColor);
    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imgLabel->setScaledContents(true);

    scrollArea = new zzScrollArea;
    scrollArea->setWidget(imgLabel);
    scrollArea->setMouseTracking(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setPalette(bg);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    setCentralWidget(scrollArea);
    connect(scrollArea, SIGNAL(sendDoubleClick()), this, SLOT(triggerFullscreen()));

    movie = new QMovie;
    movie->setCacheMode(QMovie::CacheNone);

    overlay = new infoOverlay(centralWidget());
    overlay->setHidden(true);

    cOverlay = new controlsOverlay(centralWidget());
    cOverlay->setHidden(true);

    mOverlay = new mapOverlay(centralWidget());

    connect(cOverlay, SIGNAL(exitClicked()), this, SLOT(close()));
    connect(cOverlay, SIGNAL(exitFullscreenClicked()), this, SLOT(triggerFullscreen()));
    connect(cOverlay, SIGNAL(minimizeClicked()), this, SLOT(minimizeWindow()));
    connect(imgLabel, SIGNAL(resized()), this, SLOT(updateMapOverlay()));
    connect(scrollArea, SIGNAL(scrollbarChanged()), this, SLOT(updateMapOverlay()));

    createActions();
    createMenus();

    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp";
    currentDir.setNameFilters(filters);
}

void MainWindow::changeCurrentDir(QString path) {
    currentDir.setCurrent(path);
    currentDir.setNameFilters(filters);
    fileList = currentDir.entryList();
    fInfo.fileNumber = -1;
}

void MainWindow::openDialog() {
    QString filePath = QFileDialog::getOpenFileName(this,tr("Open File"),currentDir.currentPath(),tr("Images (*.png *.jpg *jpeg *bmp *gif)"),0);//,QFileDialog::DontUseNativeDialog);
    open(filePath);
}

/* opens file specified by argument
 * stops current movie animation (if any)
 * loads either new pixmap or movie and places it into imgLabel
 * updates current fileInfo
 * updates window title, checkable menu actions
 * resizes imgLabel according to set fit options
 */
void MainWindow::open(QString filePath) {
    fInfo.setFile(&filePath);
    changeCurrentDir(fInfo.qInfo.path());
    fInfo.fileNumber = fileList.indexOf(fInfo.qInfo.fileName());
    scrollArea->repaint();
    fInfo.type = NONE;
    if (movie->state() != QMovie::NotRunning) {
        movie->stop();
    }
    if(filePath.endsWith(".gif")) {
        loadMovie(&filePath);
    }
    else {
        loadStaticImage(&filePath);
    }
    updateWindowTitle();
    updateInfoOverlay();
    scaleFactor = 1.0;
    updateActions();
    fitImage();
}

void MainWindow::loadMovie(QString *filePath) {
    movie->setFileName(*filePath);
    if(!movie->isValid()) {
        fInfo.type = NONE;
        qDebug() << "Cannot load file:" +(*filePath);
    }
    else {
        imgLabel->setMovie(movie);
        movie->start();
        fInfo.type = GIF;
        fInfo.setDimensions(movie->currentPixmap().size());
    }
}

void MainWindow::loadStaticImage(QString *filePath) {
    QImage image(*filePath);
    if(image.isNull()) {
        fInfo.type = NONE;
        qDebug() << "Cannot load file:" + (*filePath);
    }
    else {
        imgLabel->setPixmap(QPixmap::fromImage(image));
        fInfo.type = STATIC;
        fInfo.setDimensions(imgLabel->pixmap()->size());
    }
}

/* resizes imgLabel according to set fit options,
 * which is then automatically redrawn
 */
void MainWindow::fitImage() {
    if(fInfo.type != NONE) {
        double windowAspectRatio = (double)scrollArea->size().rheight()/scrollArea->size().rwidth();
        QSize newSize;
        if(fitModeAllAct->isChecked()) {
            if(fInfo.size.height()>scrollArea->size().height()
                    || fInfo.size.width()>scrollArea->size().width() ) {
                if(fInfo.aspectRatio>=windowAspectRatio) {
                    newSize.setHeight(scrollArea->size().rheight());
                    newSize.setWidth(scrollArea->size().rheight()/fInfo.aspectRatio);
                }
                else {
                    newSize.setHeight(scrollArea->size().rwidth()*fInfo.aspectRatio);
                    newSize.setWidth(scrollArea->size().rwidth());
                }
                imgLabel->setFixedSize(newSize);
            }
            else {
                imgLabel->setFixedSize(fInfo.size);
            }
        }
        else if(fitModeWidthAct->isChecked()) {
            newSize.setHeight(scrollArea->size().rwidth()*fInfo.aspectRatio);
            newSize.setWidth(scrollArea->size().rwidth());
            imgLabel->setFixedSize(newSize);
        }
        else if(fitModeNormalAct->isChecked()) {
            imgLabel->setFixedSize(fInfo.size);
        }
        else {
            imgLabel->setFixedSize(fInfo.size);
        }
    }
}

void MainWindow::fitModeAll() {
    if(fitModeAllAct->isChecked()) {
        fitModeWidthAct->setChecked(false);
        fitModeNormalAct->setChecked(false);
    }
    fitImage();
}

void MainWindow::fitModeWidth() {
    if(fitModeWidthAct->isChecked()) {
        fitModeAllAct->setChecked(false);
        fitModeNormalAct->setChecked(false);
    }
    fitImage();
}

void MainWindow::fitModeNormal() {
    if(fitModeNormalAct->isChecked()) {
        fitModeAllAct->setChecked(false);
        fitModeWidthAct->setChecked(false);
    }
    fitImage();
    scaleFactor = 1.0;
}

void MainWindow::switchFitMode() {
    if(fitModeWidthAct->isChecked()) {
        fitModeAllAct->setChecked(true);
        fitModeAll();
    }
    else if(fitModeAllAct->isChecked()) {
        fitModeNormalAct->setChecked(true);
        fitModeNormal();
    }
    else {
        fitModeAllAct->setChecked(true);
        fitModeAll();
    }
}

void MainWindow::createActions() {
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(Qt::Key_O);
    this->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(openDialog()));

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
    zoomInAct->setEnabled(false);
    this->addAction(zoomInAct);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (10%)"), this);
    zoomOutAct->setShortcut(Qt::Key_Down);
    zoomOutAct->setEnabled(false);
    this->addAction(zoomOutAct);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    fitModeNormalAct = new QAction(tr("&Normal Size"), this);
    fitModeNormalAct->setShortcut(Qt::Key_N);
    fitModeNormalAct->setEnabled(false);
    fitModeNormalAct->setCheckable(true);
    this->addAction(fitModeNormalAct);
    connect(fitModeNormalAct, SIGNAL(triggered()), this, SLOT(fitModeNormal()));

    fitModeAllAct = new QAction(tr("Fit all"), this);
    fitModeAllAct->setEnabled(false);
    fitModeAllAct->setCheckable(true);
    fitModeAllAct->setShortcut(Qt::Key_A);
    this->addAction(fitModeAllAct);
    connect(fitModeAllAct, SIGNAL(triggered()), this, SLOT(fitModeAll()));

    fitModeWidthAct = new QAction(tr("Fit &width"), this);
    fitModeWidthAct->setEnabled(false);
    fitModeWidthAct->setCheckable(true);
    fitModeWidthAct->setShortcut(Qt::Key_W);
    this->addAction(fitModeWidthAct);
    connect(fitModeWidthAct, SIGNAL(triggered()), this, SLOT(fitModeWidth()));

    fullscreenEnabledAct = new QAction(tr("&Fullscreen"), this);
    fullscreenEnabledAct->setEnabled(true);
    fullscreenEnabledAct->setCheckable(true);
    fullscreenEnabledAct->setShortcut(Qt::Key_F);
    this->addAction(fullscreenEnabledAct);
    connect(fullscreenEnabledAct, SIGNAL(triggered()), this, SLOT(switchFullscreen()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    fitModeNormalAct->setEnabled(true);
    fitModeAllAct->setEnabled(true);
    fitModeWidthAct->setEnabled(true);
}

void MainWindow::updateActions()
{
    if(fInfo.type != STATIC) {
        zoomInAct->setEnabled(false);
        zoomOutAct->setEnabled(false);
    }
    else {
        zoomInAct->setEnabled(true);
        zoomOutAct->setEnabled(true);
    }
}

void MainWindow::createMenus() {
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(fullscreenEnabledAct);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitModeNormalAct);
    viewMenu->addAction(fitModeAllAct);
    viewMenu->addAction(fitModeWidthAct);

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

void MainWindow::triggerFullscreen() {
    this->fullscreenEnabledAct->trigger();
}

void MainWindow::switchFullscreen() {
    if(fullscreenEnabledAct->isChecked()) {
        overlay->setHidden(false);
        cOverlay->setHidden(false);
        this->menuBar()->hide();
        this->showFullScreen();
    }
    else {
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
void MainWindow::next() {
    if(currentDir.exists() && fileList.length()) {
        if(++fInfo.fileNumber>=fileList.length()) {
            fInfo.fileNumber=0;
        }
        QString fName = currentDir.currentPath()+"/"+fileList.at(fInfo.fileNumber);
        fInfo.setFile(&fName);
        open(fName);
    }
}
/* same as above
 */
void MainWindow::prev() {
    if(currentDir.exists() && fileList.length()) {
        if(--fInfo.fileNumber<0) {
            fInfo.fileNumber=fileList.length()-1;
        }
        QString fName = currentDir.currentPath()+"/"+fileList.at(fInfo.fileNumber);
        fInfo.setFile(&fName);
        open(fName);
    }
}

void MainWindow::zoomIn() {
    fitModeWidthAct->setChecked(false);
    fitModeAllAct->setChecked(false);
    fitModeNormalAct->setChecked(false);
    scaleImage(1.25);
}

void MainWindow::zoomOut() {
    fitModeWidthAct->setChecked(false);
    fitModeAllAct->setChecked(false);
    fitModeNormalAct->setChecked(false);
    scaleImage(0.9);
}

/* not my code, need to review
 */
void MainWindow::scaleImage(double factor) {
    scaleFactor *= factor;
    imgLabel->setFixedSize(imgLabel->size()*factor);
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);
    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.5);
}

/* not my code, need to review
 */
void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    overlay->updateSize();
    cOverlay->updateSize();
    mOverlay->updateSize();
    updateMapOverlay();
    if(fInfo.type != NONE) {
        fitImage();
    }
}

/* calculates difference between current viewing area and total image size
 * hides overlay if image fits in view entirely
 * calculates image aspect ratio
 * calculates current view area position in percent
 * calls updateMap with above values
 */
void MainWindow::updateMapOverlay() {
    if(fInfo.type != NONE) {
        double widthDifferenceRatio=1.0;
        double heightDifferenceRatio=1.0;
        QSize imageSize = imgLabel->size();
        QSize viewportSize = scrollArea->viewport()->size();

        if(imageSize.width()>viewportSize.width())
            widthDifferenceRatio=(double)viewportSize.width()/imageSize.width();
        if(imageSize.height()>viewportSize.height())
            heightDifferenceRatio=(double)viewportSize.height()/imageSize.height();
        if(widthDifferenceRatio>=1 && heightDifferenceRatio>=1) {
            mOverlay->hide();
        }
        else {
            mOverlay->show();
            double viewportPositionX;
            double viewportPositionY;
            double imageAspectRatio = (double)imageSize.height()/imageSize.width();
            if(scrollArea->horizontalScrollBar()->maximum()==0) {
                viewportPositionX=0.0;
            }
            else {
                viewportPositionX=(double)scrollArea->horizontalScrollBar()->value()/scrollArea->horizontalScrollBar()->maximum();
            }
            if(scrollArea->verticalScrollBar()->maximum()==0) {
                viewportPositionY=0.0;
            }
            else {
                viewportPositionY=(double)scrollArea->verticalScrollBar()->value()/scrollArea->verticalScrollBar()->maximum();
            }
            mOverlay->updateMap(&widthDifferenceRatio, &heightDifferenceRatio, &viewportPositionX, &viewportPositionY, &imageAspectRatio);
        }
    }
}

void MainWindow::updateWindowTitle() {
    setWindowTitle(fInfo.getInfo()+"[ "+QString::number(fInfo.fileNumber+1)+" / "+QString::number(fileList.length())+" ] - qimgv");
}

void MainWindow::updateInfoOverlay() {
    overlay->setText(fInfo.getInfo()+"[ "+QString::number(fInfo.fileNumber+1)+" / "+QString::number(fileList.length())+" ]");
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    event->angleDelta().ry()>0?prev():next();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QMainWindow::keyPressEvent(event);
    if(event->key()==Qt::Key_Space) {
        switchFitMode();
    }
}

void MainWindow::minimizeWindow() {
    this->setWindowState(Qt::WindowMinimized);
}

MainWindow::~MainWindow()
{

}
