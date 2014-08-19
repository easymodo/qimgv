#include <QtWidgets>
#include "mainwindow.h"

MainWindow::MainWindow()
{
    init();
    fitAllAct->setChecked(true);
    changeDir("C:/Users/Практик/Desktop/share/pics/"); //starting dir
    setWindowTitle(tr("qimgv 0.04"));
    resize(800, 650);
    openDialog();
}

void MainWindow::init() {
    imgLabel = new QLabel;
    bgColor = QColor(25,25,25,255);
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
    setCentralWidget(scrollArea);
    connect(scrollArea, SIGNAL(sendDoubleClick()), this, SLOT(receiveDoubleClick()));

    movie = new QMovie;
    movie->setCacheMode(QMovie::CacheNone);

    createActions();
    createMenus();

    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp";
    currentDir.setNameFilters(filters);
    currentImgType = NONE;
}

void MainWindow::changeDir(QString path) {
    currentDir.setCurrent(path);
    currentDir.setNameFilters(filters);
    fileList = currentDir.entryList();
    fileNumber = 0;
}

void MainWindow::openDialog() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), currentDir.currentPath());
    fileInfo.setFile(filePath);
    changeDir(fileInfo.path());
    fileNumber = fileList.indexOf(fileInfo.fileName());
    open(filePath);
}

void MainWindow::open(QString filePath) {
    scrollArea->repaint();
    currentImgType = NONE;
    if (movie->state() != QMovie::NotRunning) {
        movie->stop();
    }
    if(filePath.endsWith(".gif")) {
        movie->setFileName(filePath);
        if(!movie->isValid()) {
            currentImgType = NONE;
            qDebug() << "cant load file";
        }
        else {
            imgLabel->setMovie(movie);
            movie->start();
            currentImgType = GIF;
        }
    }
    else {
        QImage image(filePath);
        if(image.isNull()) {
            currentImgType = NONE;
            qDebug() << "cant load file";
        }
        else {
            imgLabel->setPixmap(QPixmap::fromImage(image));
            currentImgType = STATIC;
        }
    }
    updateWindowTitle();
    scaleFactor = 1.0;
    updateActions();
    fitImage();
}

void MainWindow::fitImage() {
    if(currentImgType != NONE) {
        QSize currentSize;
        if(currentImgType == GIF) {
            currentSize = movie->currentPixmap().size();
        }
        else {
            currentSize = imgLabel->pixmap()->size();
        }
        double aspectRatio = (double)currentSize.rheight()/currentSize.rwidth();
        double windowAspectRatio = (double)scrollArea->size().rheight()/scrollArea->size().rwidth();
        QSize newSize;

        if(fitAllAct->isChecked()) {
            if(currentSize.height()>scrollArea->size().height()
                    || currentSize.width()>scrollArea->size().width() ) {
                if(aspectRatio>=windowAspectRatio) {
                    newSize.setHeight(scrollArea->size().rheight());
                    newSize.setWidth(scrollArea->size().rheight()/aspectRatio);
                }
                else {
                    newSize.setHeight(scrollArea->size().rwidth()*aspectRatio);
                    newSize.setWidth(scrollArea->size().rwidth());
                }
                imgLabel->setFixedSize(newSize);
            }
            else {
                imgLabel->setFixedSize(currentSize);
            }
        }
        else if(fitWidthAct->isChecked()) {
            newSize.setHeight(scrollArea->size().rwidth()*aspectRatio);
            newSize.setWidth(scrollArea->size().rwidth());
            imgLabel->setFixedSize(newSize);
        }
        else if(normalSizeAct->isChecked()) {
            imgLabel->setFixedSize(currentSize);
        }
        else {
            imgLabel->setFixedSize(currentSize);
        }
    }
}

void MainWindow::fitAll() {
    if(fitAllAct->isChecked()) {
        fitWidthAct->setChecked(false);
        normalSizeAct->setChecked(false);
    }
    fitImage();
}

void MainWindow::fitWidth() {
    if(fitWidthAct->isChecked()) {
        fitAllAct->setChecked(false);
        normalSizeAct->setChecked(false);
    }
    fitImage();
}

void MainWindow::normalSize() {
    if(normalSizeAct->isChecked()) {
        fitAllAct->setChecked(false);
        fitWidthAct->setChecked(false);
    }
    fitImage();
    scaleFactor = 1.0;
}

void MainWindow::createActions() {
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    this->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(openDialog()));


    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Alt+X"));
    this->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    nextAct = new QAction(tr("N&ext"), this);
    nextAct->setShortcut(Qt::Key_Right);
  //  nextAct->setShortcut(Qt::LeftButton);
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

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    normalSizeAct->setCheckable(true);
    this->addAction(normalSizeAct);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitAllAct = new QAction(tr("Fit to window"), this);
    fitAllAct->setEnabled(false);
    fitAllAct->setCheckable(true);
    fitAllAct->setShortcut(tr("Ctrl+F"));
    this->addAction(fitAllAct);
    connect(fitAllAct, SIGNAL(triggered()), this, SLOT(fitAll()));

    fitWidthAct = new QAction(tr("Fit to &width"), this);
    fitWidthAct->setEnabled(false);
    fitWidthAct->setCheckable(true);
    fitWidthAct->setShortcut(tr("Ctrl+W"));
    this->addAction(fitWidthAct);
    connect(fitWidthAct, SIGNAL(triggered()), this, SLOT(fitWidth()));

    switchFullscreenAct = new QAction(tr("&Fullscreen"), this);
    switchFullscreenAct->setEnabled(true);
    switchFullscreenAct->setCheckable(true);
    switchFullscreenAct->setShortcut(Qt::Key_F);
    this->addAction(switchFullscreenAct);
    connect(switchFullscreenAct, SIGNAL(triggered()), this, SLOT(switchFullscreen()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    normalSizeAct->setEnabled(true);
    fitAllAct->setEnabled(true);
    fitWidthAct->setEnabled(true);
}

void MainWindow::updateActions()
{
    if(currentImgType != STATIC) {
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
    viewMenu->addAction(switchFullscreenAct);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addSeparator();
    viewMenu->addAction(normalSizeAct);
    viewMenu->addAction(fitAllAct);
    viewMenu->addAction(fitWidthAct);

    navigationMenu = new QMenu(tr("&Navigate"), this);
    navigationMenu->addAction(nextAct);
    navigationMenu->addAction(prevAct);

    helpMenu = new QMenu(tr("&Help"), this);
    //helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(navigationMenu);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::receiveDoubleClick() {
    this->switchFullscreenAct->trigger();
}

void MainWindow::switchFullscreen() {
    if(switchFullscreenAct->isChecked()) {
        //this->hide();
        this->showFullScreen();
        this->menuBar()->hide();
    }
    else {
        this->showNormal();
        this->menuBar()->show();
    }
}

void MainWindow::next() {
    if(currentDir.exists() && fileList.length()) {
        if(++fileNumber>=fileList.length()) {
            fileNumber=0;
        }
        QString fName = currentDir.currentPath()+"/"+fileList.at(fileNumber);
        fileInfo.setFile(fName);
        open(fName);
    }
}

void MainWindow::prev() {
    if(currentDir.exists() && fileList.length()) {
        if(--fileNumber==-1) {
            fileNumber=fileList.length()-1;
        }
        QString fName = currentDir.currentPath()+"/"+fileList.at(fileNumber);
        fileInfo.setFile(fName);
        open(fName);
    }
}

void MainWindow::zoomIn() {
    scaleImage(1.1);
}

void MainWindow::zoomOut() {
    scaleImage(0.9);
}

void MainWindow::scaleImage(double factor) {
    scaleFactor *= factor;
    imgLabel->setFixedSize(imgLabel->size()*factor);
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);
    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if(currentImgType != NONE) {
        fitImage();
    }
}

void MainWindow::updateWindowTitle() {
    if(fileInfo.isFile()) {
        qint64 fsize=fileInfo.size()/1024;
        setWindowTitle(fileInfo.fileName()+" ("+QString::number(fsize)+"KB) - qimgv");
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    event->angleDelta().ry()>0?prev():next();
}

MainWindow::~MainWindow()
{

}
