#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Preferences - qimgv " +
                         QCoreApplication::applicationVersion());
    setWindowIcon(QIcon(":/images/res/pepper32.png"));
    ui->bgColorLabel->setAutoFillBackground(true);
    ui->accentColorLabel->setAutoFillBackground(true);
    connect(this, SIGNAL(settingsChanged()),
            globalSettings, SLOT(sendChangeNotification()));
    readSettings();
}

void SettingsDialog::readSettings() {

    bool setting;

    ui->infiniteScrollingCheckBox->setChecked(globalSettings->infiniteScrolling());
    ui->playVideosCheckBox->setChecked(globalSettings->playVideos());
    ui->playSoundsCheckBox->setChecked(globalSettings->playVideoSounds());

    ui->ffmpegLineEdit->setText(globalSettings->ffmpegExecutable());

    // ##### loader #####
    ui->preloaderCheckBox->setChecked(globalSettings->usePreloader());
    ui->reduceRamCheckBox->setChecked(globalSettings->reduceRamUsage());

    // ##### cache #####
    //ui->cacheSlider->setValue(globalSettings->s.value("cacheSize",64).toInt());
    ui->cacheLabel2->setNum(ui->cacheSlider->value());

    // ##### scaling #####

    setting = globalSettings->useFastScale();
    ui->scalingQualityComboBox->setCurrentIndex(setting ? 1 : 0);

    // ##### fit mode #####
    int tmp = globalSettings->imageFitMode();
    ui->fitModeComboBox->setCurrentIndex(tmp);

    // ##### UI #####
    //not implemented
    ui->scalingQualityComboBox->setDisabled(true);

    ui->fullscreenTaskbarShownCheckBox->setChecked(globalSettings->fullscreenTaskbarShown());
    ui->fullscreenCheckBox->setChecked(globalSettings->fullscreenMode());
    ui->thumbnailLabelsCheckBox->setChecked(globalSettings->showThumbnailLabels());

    ui->panelPositionComboBox->setCurrentIndex(globalSettings->panelPosition());

    //bg color
    QColor bgColor = globalSettings->backgroundColor();
    bgLabelPalette.setColor(QPalette::Window, bgColor);
    ui->bgColorLabel->setPalette(bgLabelPalette);

    //accent color
    QColor accentColor = globalSettings->accentColor();
    accentLabelPalette.setColor(QPalette::Window, accentColor);
    ui->accentColorLabel->setPalette(accentLabelPalette);

    // thumbnail size
    // maybe use slider instead of combobox?
    int size = globalSettings->thumbnailSize();
    switch(size) {
        case thumbSizeSmall: ui->thumbSizeComboBox->setCurrentIndex(0); break;
        case thumbSizeMedium: ui->thumbSizeComboBox->setCurrentIndex(1); break;
        case thumbSizeLarge: ui->thumbSizeComboBox->setCurrentIndex(2); break;
        case thumbSizeVeryLarge: ui->thumbSizeComboBox->setCurrentIndex(3); break;
        default: ui->thumbSizeComboBox->addItem("Custom: " + QString::number(size)+"px.");
                 ui->thumbSizeComboBox->setCurrentIndex(4);
                 thumbSizeCustom = size;
                 break;
    }

    // sorting mode
    int mode = globalSettings->sortingMode();
    ui->sortingComboBox->setCurrentIndex(mode);
}

void SettingsDialog::applySettings() {
    //globalSettings->s.setValue("cacheSize", ui->cacheSlider->value());
    globalSettings->setInfiniteScrolling(ui->infiniteScrollingCheckBox->isChecked());
    globalSettings->setShowThumbnailLabels(ui->thumbnailLabelsCheckBox->isChecked());
    globalSettings->setUsePreloader(ui->preloaderCheckBox->isChecked());
    globalSettings->setFullscreenMode(ui->fullscreenCheckBox->isChecked());
    globalSettings->setImageFitMode(ui->fitModeComboBox->currentIndex());
    globalSettings->setSortingMode(ui->sortingComboBox->currentIndex());
    globalSettings->setReduceRamUsage(ui->reduceRamCheckBox->isChecked());
    globalSettings->setPlayVideos(ui->playVideosCheckBox->isChecked());
    globalSettings->setPlayVideoSounds(ui->playSoundsCheckBox->isChecked());
    globalSettings->setFullscreenTaskbarShown(ui->fullscreenTaskbarShownCheckBox->isChecked());

    globalSettings->setFfmpegExecutable(ui->ffmpegLineEdit->text());

    bool useFastScale = ui->scalingQualityComboBox->currentIndex() == 1;
    globalSettings->setUseFastScale(useFastScale);

    globalSettings->setPanelPosition((PanelPosition)ui->panelPositionComboBox->currentIndex());

    globalSettings->setBackgroundColor(bgLabelPalette.color(QPalette::Window));
    globalSettings->setAccentColor(accentLabelPalette.color(QPalette::Window));

    int index = ui->thumbSizeComboBox->currentIndex();
    if(index == 0) {
        globalSettings->setThumbnailSize(thumbSizeSmall);
    } else if(index == 1) {
        globalSettings->setThumbnailSize(thumbSizeMedium);
    } else if(index == 2) {
        globalSettings->setThumbnailSize(thumbSizeLarge);
    } else if(index == 3) {
        globalSettings->setThumbnailSize(thumbSizeVeryLarge);
    } else if(index == 4) {
        globalSettings->setThumbnailSize(thumbSizeCustom);
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

void SettingsDialog::accentColorDialog() {
    QColorDialog *colorDialog = new QColorDialog(this);
    QColor newColor;
    newColor = colorDialog->getColor(accentLabelPalette.color(QPalette::Window),
                                     this,
                                     "Accent color.");
    accentLabelPalette.setColor(QPalette::Window, newColor);
    ui->accentColorLabel->setPalette(accentLabelPalette);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
