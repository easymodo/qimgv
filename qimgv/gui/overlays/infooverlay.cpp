#include "infooverlay.h"
#include "ui_infooverlay.h"

InfoOverlay::InfoOverlay(OverlayContainerWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::InfoOverlay)
{
    ui->setupUi(this);
    setPosition(FloatingWidgetPosition::TOPLEFT);
    this->setMarginX(0);
    this->setMarginY(0);

    if(parent)
        setContainerSize(parent->size());
}

InfoOverlay::~InfoOverlay()
{
    delete ui;
}

void InfoOverlay::setInfo(QString pos, QString fileName, QString info) {
    ui->posLabel->setText(pos);
    ui->nameLabel->setText(fileName);
    ui->infoLabel->setText(info);
    this->adjustSize();
}
