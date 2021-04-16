#include "resizedialog.h"
#include "ui_resizedialog.h"

ResizeDialog::ResizeDialog(QSize originalSize,  QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizeDialog),
    lastEdited(0)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    ui->percent->setFocus();

    this->originalSize = originalSize;
    targetSize = originalSize;

    ui->width->setValue(originalSize.width());
    ui->height->setValue(originalSize.height());

    ui->resetButton->setText(tr("Reset:") + " " +
                             QString::number(originalSize.width()) +
                             " x " +
                             QString::number(originalSize.height()));

    desktopSize = qApp->primaryScreen()->size();
    connect(ui->byPercentage,   &QRadioButton::toggled, this, &ResizeDialog::onPercentageRadioButton);
    connect(ui->byAbsoluteSize, &QRadioButton::toggled, this, &ResizeDialog::onAbsoluteSizeRadioButton);
    connect(ui->percent, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &ResizeDialog::percentChanged);
    connect(ui->width,  qOverload<int>(&QSpinBox::valueChanged), this, &ResizeDialog::widthChanged);
    connect(ui->height, qOverload<int>(&QSpinBox::valueChanged), this, &ResizeDialog::heightChanged);
    connect(ui->keepAspectRatio, &QCheckBox::toggled, this, &ResizeDialog::onAspectRatioCheckbox);
    connect(ui->resComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ResizeDialog::setCommonResolution);
    connect(ui->fitDesktopButton, &QPushButton::pressed, this, &ResizeDialog::fitDesktop);
    connect(ui->fillDesktopButton, &QPushButton::pressed, this, &ResizeDialog::fillDesktop);
    connect(ui->resetButton, &QPushButton::pressed, this, &ResizeDialog::reset);
    connect(ui->cancelButton, &QPushButton::pressed, this, &ResizeDialog::reject);
    connect(ui->okButton, &QPushButton::pressed, this, &ResizeDialog::sizeSelect);
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

void ResizeDialog::onAbsoluteSizeRadioButton() {
    ui->width->blockSignals(true);
    ui->height->blockSignals(true);
    ui->percent->blockSignals(true);
    ui->keepAspectRatio->blockSignals(true);

    ui->width->setEnabled(true);
    ui->height->setEnabled(true);
    ui->percent->setEnabled(false);
    ui->keepAspectRatio->setEnabled(true);

    ui->width->blockSignals(false);
    ui->height->blockSignals(false);
    ui->percent->blockSignals(false);
    ui->keepAspectRatio->blockSignals(false);
}

void ResizeDialog::onPercentageRadioButton() {
    ui->width->blockSignals(true);
    ui->height->blockSignals(true);
    ui->percent->blockSignals(true);
    ui->keepAspectRatio->blockSignals(true);

    ui->width->setEnabled(false);
    ui->height->setEnabled(false);
    ui->percent->setEnabled(true);
    ui->keepAspectRatio->setChecked(true);
    ui->keepAspectRatio->setEnabled(false);
    percentChanged(ui->percent->value());

    ui->width->blockSignals(false);
    ui->height->blockSignals(false);
    ui->percent->blockSignals(false);
    ui->keepAspectRatio->blockSignals(false);
}

void ResizeDialog::resetResCheckBox() {
    ui->resComboBox->blockSignals(true);
    ui->resComboBox->setCurrentIndex(0);
    ui->resComboBox->blockSignals(false);
}

void ResizeDialog::percentChanged(double newPercent) {
    double scale = newPercent / 100.;
    targetSize.setWidth(originalSize.width()*scale);
    targetSize.setHeight(originalSize.height()*scale);

    updateToTargetValues();
}

void ResizeDialog::keyPressEvent(QKeyEvent *event) {
    if((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        sizeSelect();
    } else if(event->key() == Qt::Key_Escape) {
        reject();
    } else {
        event->ignore();
    }
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
