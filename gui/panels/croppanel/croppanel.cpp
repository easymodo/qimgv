#include "croppanel.h"
#include "ui_croppanel.h"

CropPanel::CropPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CropPanel)
{
    ui->setupUi(this);
    this->hide();
    connect(ui->cancelButton, SIGNAL(pressed()), this, SIGNAL(cancel()));
    connect(ui->cancelButton, SIGNAL(pressed()), this, SLOT(onCropPressed()));
}

CropPanel::~CropPanel()
{
    delete ui;
}

void CropPanel::onCropPressed() {
    emit crop(cropRect);
}

void CropPanel::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
