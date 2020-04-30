#include "fullscreeninfooverlay.h"
#include "ui_fullscreeninfooverlay.h"

FullscreenInfoOverlay::FullscreenInfoOverlay(FloatingWidgetContainer *parent) :
    OverlayWidget(parent),
    ui(new Ui::FullscreenInfoOverlay)
{
    ui->setupUi(this);
    setPosition(FloatingWidgetPosition::TOPLEFT);
    this->setHorizontalMargin(0);
    this->setVerticalMargin(0);
    ui->nameLabel->setText("No file opened");
    if(parent)
        setContainerSize(parent->size());
    readSettings();
    connect(settings, &Settings::settingsChanged, this, &FullscreenInfoOverlay::readSettings);
}

void FullscreenInfoOverlay::readSettings() {
    QColor color = settings->fullscreenInfoTextColor();
    ui->posLabel->setStyleSheet("color: " + color.name() + ";");
    ui->nameLabel->setStyleSheet("color: " + color.name() + ";");
    ui->infoLabel->setStyleSheet("color: " + color.name() + ";");
}

FullscreenInfoOverlay::~FullscreenInfoOverlay() {
    delete ui;
}

void FullscreenInfoOverlay::setInfo(QString pos, QString fileName, QString info) {
    ui->posLabel->setText(pos);
    ui->nameLabel->setText(fileName);
    ui->infoLabel->setText(info);
    this->adjustSize();
}
