#include <QtWidgets>
#include "mainwindow.h"

MainWindow::MainWindow()
{
    imgLabel = new QLabel;
    imgLabel->setBackgroundRole(QPalette::Base);
    imgLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imgLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setWidget(imgLabel);
    setCentralWidget(scrollArea);

   // createActions();
  //  createMenus();

    setWindowTitle(tr("qimgv 0.01"));
    resize(500, 400);
    open();
}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), "C:/Users/Практик/Desktop/share/pics/");

            //("C:/Users/Практик/Desktop/share/tumblr_inline_mknzhrixaj1qz4rgp.png");
    QImage image(fileName);
    if(image.isNull()) QMessageBox::information(this, tr("Image Viewer"),
                                        tr("Cannot load image."));
    imgLabel->setPixmap(QPixmap::fromImage(image));
    imgLabel->adjustSize();
}

void MainWindow::close() {

}

void MainWindow::zoomIn() {

}

void MainWindow::zoomOut() {

}

MainWindow::~MainWindow()
{

}
