#include <QtWidgets>
#include "mainwindow.h"

MainWindow::MainWindow()
{
    imgLabel = new QLabel;
    QPalette bg(QColor(0,0,0,255));
    imgLabel->setBackgroundRole(QPalette::Base);
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

    movie = new QMovie;
    movie->setCacheMode(QMovie::CacheNone);

    createActions();
    createMenus();

    changeDir("C:/Users/Практик/Desktop/share/pics/"); //starting dir

    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp";
    currentDir.setNameFilters(filters);

    setWindowTitle(tr("qimgv 0.01"));
    resize(650, 500);
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
    scrollArea->repaint();
    isGif=0;
    if (movie->state() != QMovie::NotRunning) {
        movie->stop();
    }
    updateWindowTitle();
    if(filePath.endsWith(".gif")) {
        movie->setFileName(filePath);
        if(!movie->isValid()) {
            QMessageBox::information(this, tr("Image Viewer"), tr("Cannot load image."));
        }
        else {
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
    fitAllAct->setEnabled(true);
    fitWidthAct->setEnabled(true);
    updateActions();

    imgLabel->setFixedSize(movie->currentPixmap().size());
    fitImage();
}

void MainWindow::fitImage() {
    QSize currentSize;
    if(isGif) {
        currentSize = movie->currentPixmap().size();
    }
    else {
        currentSize = imgLabel->pixmap()->size();
    }
    double aspectRatio = (double)currentSize.rheight()/currentSize.rwidth();
    double windowAspectRatio = (double)scrollArea->size().rheight()/scrollArea->size().rwidth();
    QSize newSize;

    if(fitAllAct->isChecked()) {
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
    qDebug() << this->size() << scrollArea->size() << imgLabel->size();
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
    connect(openAct, SIGNAL(triggered()), this, SLOT(openDialog()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Alt+X"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    nextAct = new QAction(tr("N&ext"), this);
    nextAct->setShortcut(Qt::Key_Right);
  //  nextAct->setShortcut(Qt::LeftButton);
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
    normalSizeAct->setCheckable(true);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitAllAct = new QAction(tr("&Fit to window"), this);
    fitAllAct->setEnabled(false);
    fitAllAct->setCheckable(true);
    fitAllAct->setShortcut(tr("Ctrl+F"));
    connect(fitAllAct, SIGNAL(triggered()), this, SLOT(fitAll()));

    fitWidthAct = new QAction(tr("Fit to &width"), this);
    fitWidthAct->setEnabled(false);
    fitWidthAct->setCheckable(true);
    fitWidthAct->setShortcut(tr("Ctrl+W"));
    connect(fitWidthAct, SIGNAL(triggered()), this, SLOT(fitWidth()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::updateActions()
{
    if(!isGif) {
        zoomInAct->setEnabled(!fitAllAct->isChecked());
        zoomOutAct->setEnabled(!fitAllAct->isChecked());
        normalSizeAct->setEnabled(!fitAllAct->isChecked());
    }
    else {
        zoomInAct->setEnabled(false);
        zoomOutAct->setEnabled(false);
        normalSizeAct->setEnabled(false);
    }
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
    viewMenu->addSeparator();
    viewMenu->addAction(normalSizeAct);
    viewMenu->addAction(fitAllAct);
    viewMenu->addAction(fitWidthAct);

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
    fileInfo.setFile(fName);
    open(fName);
}

void MainWindow::zoomIn() {
    scaleImage(1.1);
}

void MainWindow::zoomOut() {
    scaleImage(0.9);
}

void MainWindow::scaleImage(double factor) {
  //  Q_ASSERT(imgLabel->pixmap());
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
    fitImage();
}

void MainWindow::updateWindowTitle() {
    if(fileInfo.isFile()) {
        qint64 fsize=fileInfo.size()/1024;
        setWindowTitle(fileInfo.fileName()+" ("+QString::number(fsize)+"KB) - qimgv");
    }


}

MainWindow::~MainWindow()
{

}
