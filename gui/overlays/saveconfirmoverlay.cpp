#include "saveconfirmoverlay.h"
#include "ui_saveconfirmoverlay.h"

SaveConfirmOverlay::SaveConfirmOverlay(QWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::SaveConfirmOverlay)
{
    this->position = FloatingWidgetPosition::BOTTOMRIGHT;
    ui->setupUi(this);
    connect(ui->saveButton, SIGNAL(clicked()), this, SIGNAL(saveClicked()));
    connect(ui->saveAsButton, SIGNAL(clicked()), this, SIGNAL(saveAsClicked()));
    connect(ui->discardButton, SIGNAL(clicked()), this, SIGNAL(discardClicked()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SIGNAL(hide()));
    connect(ui->saveAsButton, SIGNAL(clicked()), this, SIGNAL(hide()));
    connect(ui->discardButton, SIGNAL(clicked()), this, SIGNAL(hide()));
    this->hide();
}

SaveConfirmOverlay::~SaveConfirmOverlay()
{
    delete ui;
}
