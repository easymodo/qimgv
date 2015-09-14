#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Settings"));
    setWindowIcon(QIcon(":/images/res/pepper32.png"));
    ui->bgColorLabel->setAutoFillBackground(true);
    connect(this, SIGNAL(settingsChanged()),
            globalSettings, SLOT(sendChangeNotification()));
    readSettings();
}

void SettingsDialog::readSettings() {
    QString tmp;
    bool setting;

    // ##### loader #####
    ui->preloaderCheckBox->setChecked(
                globalSettings->s.value("usePreloader", "true").toBool());

    ui->fullscreenCheckBox->setChecked(
                globalSettings->s.value("openInFullscreen", "true").toBool());

    // ##### cache #####
    ui->cacheSlider->setValue(
                globalSettings->s.value("cacheSize",64).toInt());
    ui->cacheLabel2->setNum(ui->cacheSlider->value());

    // ##### scaling #####

    setting = globalSettings->s.value("useFastScale","false").toBool();
    if(setting==true) {
        ui->scalingQualityComboBox->setCurrentIndex(1);
    } else {
        ui->scalingQualityComboBox->setCurrentIndex(0);
    }

    // ##### fit mode #####
    tmp = globalSettings->s.value("defaultFitMode","ALL").toString();
    if(tmp == "WIDTH") {
        ui->fitModeComboBox->setCurrentIndex(1);
    }
    else if(tmp == "NORMAL") {
        ui->fitModeComboBox->setCurrentIndex(2);
    }
    else {
        ui->fitModeComboBox->setCurrentIndex(0);
    }

    // ##### UI #####

    //not implemented
    ui->scalingQualityComboBox->setDisabled(true);

    //bg color
    QColor bgColor = globalSettings->s.value("bgColor", "Qt::black").value<QColor>();
    bgLabelPalette.setColor(QPalette::Window, bgColor);
    ui->bgColorLabel->setPalette(bgLabelPalette);

    // thumbnail size
    // maybe use slider instead of combobox?
    int size = globalSettings->s.value("thumbnailSize", "120").toInt();
    switch(size) {
        case thumbSizeSmall: ui->thumbSizeComboBox->setCurrentIndex(0); break;
        case thumbSizeMedium: ui->thumbSizeComboBox->setCurrentIndex(1); break;
        case thumbSizeLarge: ui->thumbSizeComboBox->setCurrentIndex(2); break;
        case thumbSizeVeryLarge: ui->thumbSizeComboBox->setCurrentIndex(3); break;
        default:  ui->thumbSizeComboBox->addItem("Custom: " + QString::number(size)+"px.");
                  ui->thumbSizeComboBox->setCurrentIndex(4);
                  thumbSizeCustom = size;
                  break;
    }

    // sorting mode
    int mode = globalSettings->sortingMode();
    ui->sortingComboBox->setCurrentIndex(mode);
}

void SettingsDialog::applySettings() {
    globalSettings->s.setValue("usePreloader",
                            ui->preloaderCheckBox->isChecked());
    globalSettings->s.setValue("openInFullscreen",
                            ui->fullscreenCheckBox->isChecked());
    globalSettings->s.setValue("cacheSize",
                               ui->cacheSlider->value());
    globalSettings->s.setValue("defaultFitMode",
                               ui->fitModeComboBox->currentText());
    globalSettings->setSortingMode(ui->sortingComboBox->currentIndex());

    if(ui->scalingQualityComboBox->currentIndex()==1) {
        globalSettings->s.setValue("useFastScale", true);
    } else {
        globalSettings->s.setValue("useFastScale", false);
    }
    globalSettings->s.setValue("bgColor",
                               bgLabelPalette.color(QPalette::Window));

    if(ui->thumbSizeComboBox->currentIndex() == 0) {
        globalSettings->s.setValue("thumbnailSize", thumbSizeSmall);
    } else if(ui->thumbSizeComboBox->currentIndex() == 1) {
        globalSettings->s.setValue("thumbnailSize", thumbSizeMedium);
    } else if(ui->thumbSizeComboBox->currentIndex() == 2) {
        globalSettings->s.setValue("thumbnailSize", thumbSizeLarge);
    } else if(ui->thumbSizeComboBox->currentIndex() == 3) {
        globalSettings->s.setValue("thumbnailSize", thumbSizeVeryLarge);
    } else if(ui->thumbSizeComboBox->currentIndex() == 4) {
        globalSettings->s.setValue("thumbnailSize", thumbSizeCustom);
    }
    emit settingsChanged();
}

void SettingsDialog::applySettingsAndClose() {
    applySettings();
    this->close();
}

void SettingsDialog::bgColorDialog() {
    QColorDialog *colorDialog = new QColorDialog(this);
    QColor newColor;
    newColor = colorDialog->getColor(bgLabelPalette.color(QPalette::Window),
                                     this,
                                     "Background color.");
    bgLabelPalette.setColor(QPalette::Window, newColor);
    ui->bgColorLabel->setPalette(bgLabelPalette);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
