#include "imageinfooverlay.h"
#include "ui_imageinfooverlay.h"

ImageInfoOverlay::ImageInfoOverlay(OverlayContainerWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::ImageInfoOverlay)
{
    ui->setupUi(this);
    hide();
    ui->tableWidget->setColumnWidth(0, 120);
    ui->tableWidget->setColumnWidth(1, 142);
    this->setPosition(FloatingWidgetPosition::RIGHT);

    if(parent)
        setContainerSize(parent->size());
}

ImageInfoOverlay::~ImageInfoOverlay() {
    delete ui;
}

void ImageInfoOverlay::setExifInfo(QMap<QString, QString> info) {
    while(ui->tableWidget->rowCount() > 0) {
        ui->tableWidget->removeRow(ui->tableWidget->rowCount() - 1);
    }
    QMap<QString, QString>::const_iterator i = info.constBegin();
    while (i != info.constEnd()) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(i.key()));
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, new QTableWidgetItem(i.value()));
        ++i;
    }
}

void ImageInfoOverlay::wheelEvent(QWheelEvent *event) {
    event->accept();
}
