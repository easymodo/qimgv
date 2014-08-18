#include <QtWidgets>
#include "mainwindow.h"

MainWindow::MainWindow()
{
    imgLabel = new QLabel;
    imgLabel->setBackgroundRole(QPalette::Base);
    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imgLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setWidget(imgLabel);
    scrollArea->setMouseTracking(true);
    setCentralWidget(scrollArea);

    movie = new QMovie;
    movie->setCacheMode(QMovie::CacheNone);

    createActions();
    createMenus();

    changeDir("C:/Users/Практик/Desktop/share/pics/"); //starting dir

    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp";
    currentDir.setNameFilters(filters);

    setWindowTitle(tr("qimgv 0.01"));
    resize(500, 400);
    openDialog();

}

void MainWindow::changeDir(QString path) {
    currentDir.setCurrent(path);
    currentDir.setNameFilters(filters);
    fileNumber = 0;
}

void MainWindow::openDialog() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), currentDir.currentPath());
    fileInfo.setFile(filePath);
    changeDir(fileInfo.path());
    fileList = currentDir.entryList();
    fileNumber = fileList.indexOf(fileInfo.fileName());
    open(filePath);
}

void MainWindow::open(QString filePath) {
    isGif=0;
    if (movie->state() != QMovie::NotRunning) {
        movie->stop();
    }
    setWindowTitle(fileInfo.fileName()+" - qimgv");
   // qDebug() << "showing image " + fileInfo.filePath();
    if(filePath.endsWith(".gif")) {
     //   qDebug() << filePath;
        movie->setFileName(filePath);
        if(!movie->isValid()) {
            QMessageBox::information(this, tr("Image Viewer"), tr("Cannot load image."));
        }
        else {
            gifFrameCount = movie->frameCount();
            imgLabel->setMovie(movie);
            movie->start();
            isGif=1;
        }
    }
    else {
        QImage image(filePath);
        if(image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"), tr("Cannot load image."));
        }
        else {
            imgLabel->setPixmap(QPixmap::fromImage(image));
        }
    }
    scaleFactor = 1.0;
    fitToWidthAct->setEnabled(true);
    updateActions();

    if (!fitToWidthAct->isChecked())
        imgLabel->adjustSize();
}

void MainWindow::createActions() {
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openDialog()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Alt+X"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    nextAct = new QAction(tr("N&ext"), this);
    nextAct->setShortcut(Qt::Key_Right);
    nextAct->setEnabled(true);
    connect(nextAct, SIGNAL(triggered()), this, SLOT(next()));

    prevAct = new QAction(tr("P&rev"), this);
    prevAct->setShortcut(Qt::Key_Left);
    connect(prevAct, SIGNAL(triggered()), this, SLOT(prev()));

    zoomInAct = new QAction(tr("Zoom &In (10%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (10%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWidthAct = new QAction(tr("&Fit to Width"), this);
    fitToWidthAct->setEnabled(false);
    fitToWidthAct->setCheckable(true);
    fitToWidthAct->setShortcut(tr("Ctrl+F"));
    connect(fitToWidthAct, SIGNAL(triggered()), this, SLOT(fitToWidth()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::updateActions()
{
    zoomInAct->setEnabled(!fitToWidthAct->isChecked());
    zoomOutAct->setEnabled(!fitToWidthAct->isChecked());
    normalSizeAct->setEnabled(!fitToWidthAct->isChecked());
}

void MainWindow::createMenus() {
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    fileMenu->addAction(nextAct);
    fileMenu->addAction(prevAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWidthAct);

    helpMenu = new QMenu(tr("&Help"), this);
    //helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::next() {
    if(++fileNumber>=fileList.length()) {
        fileNumber=0;
    }
    QString fName = currentDir.currentPath()+"/"+fileList.at(fileNumber);
    fileInfo.setFile(fName);
    open(fName);
}

void MainWindow::prev() {
    if(--fileNumber==-1) {
        fileNumber=fileList.length()-1;
    }
    QString fName = currentDir.currentPath()+"/"+fileList.at(fileNumber);
   // qDebug() << fName;
    fileInfo.setFile(fName);
    open(fName);
}

void MainWindow::fitToWidth() {

}

void MainWindow::normalSize() {
    imgLabel->adjustSize();
    scaleFactor = 1.0;
}

void MainWindow::zoomIn() {
    scaleImage(1.1);
}

void MainWindow::zoomOut() {
    scaleImage(0.9);
}

void MainWindow::scaleImage(double factor) {
    if(!isGif) {
        Q_ASSERT(imgLabel->pixmap());
        scaleFactor *= factor;
        imgLabel->resize(scaleFactor * imgLabel->pixmap()->size());
        adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
        adjustScrollBar(scrollArea->verticalScrollBar(), factor);
        zoomInAct->setEnabled(scaleFactor < 3.0);
        zoomOutAct->setEnabled(scaleFactor > 0.333);
    }
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

MainWindow::~MainWindow()
{

}
