#include "croppanel.h"
#include "ui_croppanel.h"

CropPanel::CropPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CropPanel)
{
    ui->setupUi(this);
    this->hide();
    connect(ui->cancelButton, SIGNAL(pressed()), this, SIGNAL(cancel()));
    connect(ui->cropButton, SIGNAL(pressed()), this, SLOT(onCropPressed()));
    connect(ui->width, SIGNAL(valueChanged(int)), this, SLOT(onSelectionChange()));
    connect(ui->height, SIGNAL(valueChanged(int)), this, SLOT(onSelectionChange()));
    connect(ui->posX, SIGNAL(valueChanged(int)), this, SLOT(onSelectionChange()));
    connect(ui->posY, SIGNAL(valueChanged(int)), this, SLOT(onSelectionChange()));
}

CropPanel::~CropPanel()
{
    delete ui;
}

void CropPanel::onCropPressed() {
    emit cropClicked();
}

// on user input
void CropPanel::onSelectionChange() {
    emit selectionChanged(QRect(ui->posX->value(),
                                ui->posY->value(),
                                ui->width->value(),
                                ui->height->value())
                          );
}

// update input box values
void CropPanel::onSelectionOutsideChange(QRect rect) {
    ui->width->blockSignals(true);
    ui->height->blockSignals(true);
    ui->posX->blockSignals(true);
    ui->posY->blockSignals(true);

    ui->width->setValue(rect.width());
    ui->height->setValue(rect.height());
    ui->posX->setValue(rect.left());
    ui->posY->setValue(rect.top());

    ui->width->blockSignals(false);
    ui->height->blockSignals(false);
    ui->posX->blockSignals(false);
    ui->posY->blockSignals(false);
}

void CropPanel::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
