#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    this->setWindowTitle("Preferences - qimgv " +
                         QCoreApplication::applicationVersion());
    setWindowIcon(QIcon(":/images/res/pepper32.png"));
    ui->bgColorLabel->setAutoFillBackground(true);
    ui->accentColorLabel->setAutoFillBackground(true);
    shortcutKeys = actionManager->keys();
    ui->versionLabel->setText(QApplication::applicationVersion());
    connect(this, SIGNAL(settingsChanged()),
            settings, SLOT(sendChangeNotification()));
    readSettings();
}

void SettingsDialog::readSettings() {

    bool setting;

    ui->infiniteScrollingCheckBox->setChecked(settings->infiniteScrolling());
    ui->playVideosCheckBox->setChecked(settings->playVideos());
    ui->playSoundsCheckBox->setChecked(settings->playVideoSounds());
    ui->showMenuBarCheckBox->setChecked(!settings->menuBarHidden());
    ui->enablePanelCheckBox->setChecked(settings->panelEnabled());

    ui->ffmpegLineEdit->setText(settings->ffmpegExecutable());

    // ##### loader #####
    ui->preloaderCheckBox->setChecked(settings->usePreloader());
    ui->reduceRamCheckBox->setChecked(settings->reduceRamUsage());

    // ##### cache #####
    //ui->cacheSlider->setValue(globalSettings->s.value("cacheSize",64).toInt());
    ui->cacheLabel2->setNum(ui->cacheSlider->value());

    // ##### scaling #####

    setting = settings->useFastScale();
    ui->scalingQualityComboBox->setCurrentIndex(setting ? 1 : 0);

    // ##### fit mode #####
    int tmp = settings->imageFitMode();
    ui->fitModeComboBox->setCurrentIndex(tmp);

    // ##### UI #####
    //not implemented
    //ui->scalingQualityComboBox->setDisabled(true);

    ui->fullscreenTaskbarShownCheckBox->setChecked(settings->fullscreenTaskbarShown());
    ui->fullscreenCheckBox->setChecked(settings->fullscreenMode());
    ui->thumbnailLabelsCheckBox->setChecked(settings->showThumbnailLabels());

    ui->panelPositionComboBox->setCurrentIndex(settings->panelPosition());

    //bg color
    QColor bgColor = settings->backgroundColor();
    bgLabelPalette.setColor(QPalette::Window, bgColor);
    ui->bgColorLabel->setPalette(bgLabelPalette);

    //accent color
    QColor accentColor = settings->accentColor();
    accentLabelPalette.setColor(QPalette::Window, accentColor);
    ui->accentColorLabel->setPalette(accentLabelPalette);

    // thumbnail size
    // maybe use slider instead of combobox?
    int size = settings->thumbnailSize();
    switch(size) {
        case SettingsDialog::thumbSizeSmall:
            ui->thumbSizeComboBox->setCurrentIndex(0);
            break;
        case SettingsDialog::thumbSizeMedium:
            ui->thumbSizeComboBox->setCurrentIndex(1);
            break;
        case SettingsDialog::thumbSizeLarge:
            ui->thumbSizeComboBox->setCurrentIndex(2);
            break;
        case SettingsDialog::thumbSizeVeryLarge:
            ui->thumbSizeComboBox->setCurrentIndex(3);
            break;
        default:
            ui->thumbSizeComboBox->addItem("Custom: " + QString::number(size) + "px");
            ui->thumbSizeComboBox->setCurrentIndex(4);
            thumbSizeCustom = size;
            break;
    }

    // sorting mode
    int mode = settings->sortingMode();
    ui->sortingComboBox->setCurrentIndex(mode);

    fillShortcuts();
}

void SettingsDialog::applySettings() {
    //globalSettings->s.setValue("cacheSize", ui->cacheSlider->value());
    settings->setInfiniteScrolling(ui->infiniteScrollingCheckBox->isChecked());
    settings->setShowThumbnailLabels(ui->thumbnailLabelsCheckBox->isChecked());
    settings->setUsePreloader(ui->preloaderCheckBox->isChecked());
    settings->setFullscreenMode(ui->fullscreenCheckBox->isChecked());
    settings->setImageFitMode(ui->fitModeComboBox->currentIndex());
    settings->setSortingMode(ui->sortingComboBox->currentIndex());
    settings->setReduceRamUsage(ui->reduceRamCheckBox->isChecked());
    settings->setPlayVideos(ui->playVideosCheckBox->isChecked());
    settings->setPlayVideoSounds(ui->playSoundsCheckBox->isChecked());
    settings->setFullscreenTaskbarShown(ui->fullscreenTaskbarShownCheckBox->isChecked());
    settings->setMenuBarHidden(!ui->showMenuBarCheckBox->isChecked());
    settings->setPanelEnabled(ui->enablePanelCheckBox->isChecked());

    settings->setFfmpegExecutable(ui->ffmpegLineEdit->text());

    bool useFastScale = ui->scalingQualityComboBox->currentIndex() == 1;
    settings->setUseFastScale(useFastScale);

    settings->setPanelPosition((PanelPosition) ui->panelPositionComboBox->currentIndex());

    settings->setBackgroundColor(bgLabelPalette.color(QPalette::Window));
    settings->setAccentColor(accentLabelPalette.color(QPalette::Window));

    int index = ui->thumbSizeComboBox->currentIndex();
    if(index == 0) {
        settings->setThumbnailSize(thumbSizeSmall);
    } else if(index == 1) {
        settings->setThumbnailSize(thumbSizeMedium);
    } else if(index == 2) {
        settings->setThumbnailSize(thumbSizeLarge);
    } else if(index == 3) {
        settings->setThumbnailSize(thumbSizeVeryLarge);
    } else if(index == 4) {
        settings->setThumbnailSize(thumbSizeCustom);
    }

    applyShortcuts();
    settings->saveShortcuts();
    emit settingsChanged();
}

void SettingsDialog::applySettingsAndClose() {
    applySettings();
    this->close();
}

void SettingsDialog::fillShortcuts() {
    ui->shortcutsListWidget->clear();
    const QMap<QString, QString> shortcuts = actionManager->allShortcuts();
    QMapIterator<QString, QString> i(shortcuts);
    while(i.hasNext()) {
        i.next();
        ui->shortcutsListWidget->addItem(i.value() + "=" + i.key());
    }
}

void SettingsDialog::addShortcut() {
    const QStringList actionList = actionManager->actionList(); // move to constructor?
    SettingsShortcutWidget w(actionList, shortcutKeys, this);
    if(w.exec()) {
        ui->shortcutsListWidget->addItem(w.text());
    }
}

void SettingsDialog::removeShortcut() {
    int row = ui->shortcutsListWidget->currentRow();
    if(row >= 0) {
        delete ui->shortcutsListWidget->takeItem(row);
    }
}

void SettingsDialog::applyShortcuts() {
    actionManager->removeAll();
    for(int i = 0; i < ui->shortcutsListWidget->count(); i++) {
        QStringList s = ui->shortcutsListWidget->item(i)->text().split("=");
        actionManager->addShortcut(s[1], s[0]);
    }
}

void SettingsDialog::resetShortcuts() {
    actionManager->resetDefaults();
    fillShortcuts();
}

void SettingsDialog::selectFFMPEG() {
    QFileDialog dialog;
    QString file;
    file = dialog.getOpenFileName(this, "Navigate to ffmpeg.exe", "", "ffmpeg.exe");
    if(!file.isEmpty()) {
        ui->ffmpegLineEdit->setText(file);
    }
}

void SettingsDialog::bgColorDialog() {
    QColorDialog *colorDialog = new QColorDialog(this);
    QColor newColor;
    newColor = colorDialog->getColor(bgLabelPalette.color(QPalette::Window),
                                     this,
                                     "Background color.");
    if(newColor.isValid()) {
        bgLabelPalette.setColor(QPalette::Window, newColor);
        ui->bgColorLabel->setPalette(bgLabelPalette);
    }
    delete colorDialog;
}

void SettingsDialog::accentColorDialog() {
    QColorDialog *colorDialog = new QColorDialog(this);
    QColor newColor;
    newColor = colorDialog->getColor(accentLabelPalette.color(QPalette::Window),
                                     this,
                                     "Accent color.");
    if(newColor.isValid()) {
        accentLabelPalette.setColor(QPalette::Window, newColor);
        ui->accentColorLabel->setPalette(accentLabelPalette);
    }
    delete colorDialog;
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
