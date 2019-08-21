#include "resizedialog.h"
#include "ui_resizedialog.h"

ResizeDialog::ResizeDialog(QSize originalSize,  QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizeDialog),
    lastEdited(0)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Resize image");

    ui->width->setFocus();

    this->originalSize = originalSize;
    targetSize = originalSize;

    ui->width->setValue(originalSize.width());
    ui->height->setValue(originalSize.height());

    ui->resetButton->setText("Reset: " +
                             QString::number(originalSize.width()) +
                             " x " +
                             QString::number(originalSize.height()));

    QDesktopWidget desktopWidget;
    desktopSize = desktopWidget.screenGeometry(desktopWidget.primaryScreen()).size();
    connect(ui->width, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));
    connect(ui->height, SIGNAL(valueChanged(int)), this, SLOT(heightChanged(int)));
    connect(ui->keepAspectRatio, SIGNAL(toggled(bool)), this, SLOT(onAspectRatioCheckbox()));
    connect(ui->resComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setCommonResolution(int)));
    connect(ui->fitDesktopButton, SIGNAL(pressed()), this, SLOT(fitDesktop()));
    connect(ui->fillDesktopButton, SIGNAL(pressed()), this, SLOT(fillDesktop()));
    connect(ui->resetButton, SIGNAL(pressed()), this, SLOT(reset()));
    connect(ui->cancelButton, SIGNAL(pressed()), this, SLOT(reject()));
    connect(ui->okButton, SIGNAL(pressed()), this, SLOT(sizeSelect()));
}

ResizeDialog::~ResizeDialog() {
    delete ui;
}

void ResizeDialog::sizeSelect() {
    if(targetSize != originalSize)
        emit sizeSelected(targetSize);
    this->accept();
}

void ResizeDialog::setCommonResolution(int index) {
    QSize res;
    switch(index) {
        case 1: res = QSize(1366, 768); break;
        case 2: res = QSize(1440, 900); break;
        case 3: res = QSize(1440, 1050); break;
        case 4: res = QSize(1600, 1200); break;
        case 5: res = QSize(1920, 1080); break;
        case 6: res = QSize(1920, 1200); break;
        case 7: res = QSize(2560, 1080); break;
        case 8: res = QSize(2560, 1440); break;
        case 9: res = QSize(2560, 1600); break;
        case 10: res = QSize(3840, 1600); break;
        case 11: res = QSize(3840, 2160); break;
        default: res = originalSize; break;
    }
    if(ui->keepAspectRatio->isChecked())
        targetSize = originalSize.scaled(res, Qt::KeepAspectRatio);
    else
        targetSize = originalSize.scaled(res, Qt::IgnoreAspectRatio);
    updateToTargetValues();
}

QSize ResizeDialog::newSize() {
    return targetSize;
}

void ResizeDialog::widthChanged(int newWidth) {
    lastEdited = 0;
    float factor = static_cast<float>(newWidth) / originalSize.width();
    targetSize.setWidth(newWidth);
    if(ui->keepAspectRatio->isChecked()) {
        targetSize.setHeight(static_cast<int>(originalSize.height() * factor));
    }
    updateToTargetValues();
}

void ResizeDialog::heightChanged(int newHeight) {
    lastEdited = 1;
    float factor = static_cast<float>(newHeight) / originalSize.height();
    targetSize.setHeight(newHeight);
    if(ui->keepAspectRatio->isChecked()) {
        targetSize.setWidth(static_cast<int>(originalSize.width() * factor));
    }
    updateToTargetValues();
}

void ResizeDialog::updateToTargetValues() {
    ui->width->blockSignals(true);
    ui->height->blockSignals(true);
    ui->width->setValue(targetSize.width());
    ui->height->setValue(targetSize.height());
    ui->width->blockSignals(false);
    ui->height->blockSignals(false);
}

void ResizeDialog::fitDesktop() {
    targetSize = originalSize.scaled(desktopSize, Qt::KeepAspectRatio);
    updateToTargetValues();
}

void ResizeDialog::fillDesktop() {
    targetSize = originalSize.scaled(desktopSize, Qt::KeepAspectRatioByExpanding);
    updateToTargetValues();
}

void ResizeDialog::onAspectRatioCheckbox() {
    resetResCheckBox();
    (lastEdited)?heightChanged(ui->height->value()):widthChanged(ui->width->value());
}

void ResizeDialog::resetResCheckBox() {
    ui->resComboBox->blockSignals(true);
    ui->resComboBox->setCurrentIndex(0);
    ui->resComboBox->blockSignals(false);
}

void ResizeDialog::reset() {
    resetResCheckBox();
    targetSize = originalSize;
    updateToTargetValues();
}

int ResizeDialog::exec() {
    resize(sizeHint());
    return QDialog::exec();
}
