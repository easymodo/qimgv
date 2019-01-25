#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Preferences - qimgv");
    ui->shortcutsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->windowColorLabel->setAutoFillBackground(true);
    ui->fullscreenColorLabel->setAutoFillBackground(true);
    ui->accentColorLabel->setAutoFillBackground(true);
    ui->aboutAppTextBrowser->viewport()->setAutoFillBackground(false);
    ui->versionLabel->setText("" + QApplication::applicationVersion());
    ui->qtVersionLabel->setText(qVersion());
    ui->appIconLabel->setPixmap(QIcon(":/res/icons/app/22.png").pixmap(22,22));
    ui->qtIconLabel->setPixmap(QIcon(":/res/icons/qt22.png").pixmap(22,16));

#ifndef USE_KDE_BLUR
    ui->blurBackgroundCheckBox->setEnabled(false);
#endif

#ifndef USE_MPV
    ui->videoSettingsWidget->setEnabled(false);
    ui->novideoInfoLabel->setHidden(false);
#else
    ui->novideoInfoLabel->setHidden(true);
#endif

    setupSidebar();

    connect(this, SIGNAL(settingsChanged()),
            settings, SLOT(sendChangeNotification()));
    readSettings();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::setupSidebar() {
    QListWidget *sideBar = ui->sideBar;
    sideBar->viewport()->setAutoFillBackground(false);
    // General
    sideBar->item(0)->setIcon(QIcon(":/res/icons/settings/32/tweak32.png"));
    // Appearance
    sideBar->item(1)->setIcon(QIcon(":/res/icons/settings/32/appearance32.png"));
    // Scaling
    sideBar->item(2)->setIcon(QIcon(":/res/icons/settings/32/scale32.png"));
    // Controls
    sideBar->item(3)->setIcon(QIcon(":/res/icons/settings/32/shortcuts32.png"));
    // Scripts
    sideBar->item(4)->setIcon(QIcon(":/res/icons/settings/32/terminal32.png"));
    // Advanced
    sideBar->item(5)->setIcon(QIcon(":/res/icons/settings/32/preferences32.png"));
    // About
    sideBar->item(6)->setIcon(QIcon(":/res/icons/app/32.png"));

#ifdef _WIN32
    // Not implemented on windows. Not sure if will ever be. I don't really care.
    sideBar->item(4)->setHidden(true);
#endif
}

void SettingsDialog::readSettings() {
    bool setting;
    ui->infiniteScrollingCheckBox->setChecked(settings->infiniteScrolling());
    ui->playWebmCheckBox->setChecked(settings->playWebm());
    ui->playMp4CheckBox->setChecked(settings->playMp4());
    ui->playSoundsCheckBox->setChecked(settings->playVideoSounds());
    ui->enablePanelCheckBox->setChecked(settings->panelEnabled());
    ui->panelFullscreenOnlyCheckBox->setChecked(settings->panelFullscreenOnly());
    ui->mouseWrappingCheckBox->setChecked(settings->mouseWrapping());
    ui->squareThumbnailsCheckBox->setChecked(settings->squareThumbnails());
    ui->transparencyGridCheckBox->setChecked(settings->transparencyGrid());
    ui->enableSmoothScrollCheckBox->setChecked(settings->enableSmoothScroll());
    ui->usePreloaderCheckBox->setChecked(settings->usePreloader());
    ui->useThumbnailCacheCheckBox->setChecked(settings->useThumbnailCache());
    ui->smoothUpscalingCheckBox->setChecked(settings->smoothUpscaling());
    ui->expandImageCheckBox->setChecked(settings->expandImage());
    ui->smoothAnimatedImagesCheckBox->setChecked(settings->smoothAnimatedImages());
    ui->bgOpacitySlider->setValue(static_cast<int>(settings->backgroundOpacity() * 100));
    ui->blurBackgroundCheckBox->setChecked(settings->blurBackground());
    ui->sortingComboBox->setCurrentIndex(settings->sortingMode());
    ui->showInfoOverlayCheckBox->setChecked(settings->showInfoOverlay());

    ui->mpvLineEdit->setText(settings->mpvBinary());

    // ##### scaling #####
    setting = settings->useFastScale();
    ui->scalingQualityComboBox->setCurrentIndex(setting ? 1 : 0);
    ui->maxZoomSlider->setValue(settings->maximumZoom());
    onMaxZoomSliderChanged(settings->maximumZoom());
    ui->maxZoomResSlider->setValue(settings->maxZoomedResolution());
    onMaxZoomResolutionSliderChanged(settings->maxZoomedResolution());

    // thumbnailer threads
    ui->thumbnailerThreadsSlider->setValue(settings->thumbnailerThreadCount());
    onThumbnailerThreadsSliderChanged(settings->thumbnailerThreadCount());

    // ##### fit mode #####
    int fitMode = settings->imageFitMode();
    ui->fitModeComboBox->setCurrentIndex(fitMode);

    // ##### UI #####
    ui->scalingQualityComboBox->setCurrentIndex(settings->scalingFilter());
    ui->fullscreenCheckBox->setChecked(settings->fullscreenMode());
    ui->panelPositionComboBox->setCurrentIndex(settings->panelPosition());

    //bg colors
    QColor windowColor = settings->backgroundColor();
    windowColorPalette.setColor(QPalette::Window, windowColor);
    ui->windowColorLabel->setPalette(windowColorPalette);

    QColor fullscreenColor = settings->backgroundColorFullscreen();
    fullscreenColorPalette.setColor(QPalette::Window, fullscreenColor);
    ui->fullscreenColorLabel->setPalette(fullscreenColorPalette);

    //accent color
    QColor accentColor = settings->accentColor();
    accentLabelPalette.setColor(QPalette::Window, accentColor);
    ui->accentColorLabel->setPalette(accentLabelPalette);

    // thumbnail size
    // maybe use slider instead of combobox?
    unsigned int size = settings->mainPanelSize();
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
    populateShortcuts();
    populateScripts();
}

void SettingsDialog::applySettings() {
    // wait for all background stuff to finish
    if(QThreadPool::globalInstance()->activeThreadCount()) {
        QThreadPool::globalInstance()->waitForDone();
    }

    settings->setInfiniteScrolling(ui->infiniteScrollingCheckBox->isChecked());
    settings->setFullscreenMode(ui->fullscreenCheckBox->isChecked());
    settings->setImageFitMode(static_cast<ImageFitMode>(ui->fitModeComboBox->currentIndex()));
    settings->setPlayWebm(ui->playWebmCheckBox->isChecked());
    settings->setPlayMp4(ui->playMp4CheckBox->isChecked());
    settings->setPlayVideoSounds(ui->playSoundsCheckBox->isChecked());
    settings->setPanelEnabled(ui->enablePanelCheckBox->isChecked());
    settings->setPanelFullscreenOnly(ui->panelFullscreenOnlyCheckBox->isChecked());
    settings->setMouseWrapping(ui->mouseWrappingCheckBox->isChecked());
    settings->setSquareThumbnails(ui->squareThumbnailsCheckBox->isChecked());
    settings->setTransparencyGrid(ui->transparencyGridCheckBox->isChecked());
    settings->setEnableSmoothScroll(ui->enableSmoothScrollCheckBox->isChecked());
    settings->setUsePreloader(ui->usePreloaderCheckBox->isChecked());
    settings->setUseThumbnailCache(ui->useThumbnailCacheCheckBox->isChecked());
    settings->setSmoothUpscaling(ui->smoothUpscalingCheckBox->isChecked());
    settings->setExpandImage(ui->expandImageCheckBox->isChecked());
    settings->setSmoothAnimatedImages(ui->smoothAnimatedImagesCheckBox->isChecked());
    settings->setBackgroundOpacity(static_cast<qreal>(ui->bgOpacitySlider->value()) / 100);
    settings->setBlurBackground(ui->blurBackgroundCheckBox->isChecked());
    settings->setSortingMode(static_cast<SortingMode>(ui->sortingComboBox->currentIndex()));
    settings->setShowInfoOverlay(ui->showInfoOverlayCheckBox->isChecked());

    settings->setMpvBinary(ui->mpvLineEdit->text());

    settings->setScalingFilter(ui->scalingQualityComboBox->currentIndex());

    settings->setPanelPosition(static_cast<PanelHPosition>(ui->panelPositionComboBox->currentIndex()));

    settings->setBackgroundColor(windowColorPalette.color(QPalette::Window));
    settings->setBackgroundColorFullscreen(fullscreenColorPalette.color(QPalette::Window));
    settings->setAccentColor(accentLabelPalette.color(QPalette::Window));

    int index = ui->thumbSizeComboBox->currentIndex();
    if(index == 0) {
        settings->setMainPanelSize(thumbSizeSmall);
    } else if(index == 1) {
        settings->setMainPanelSize(thumbSizeMedium);
    } else if(index == 2) {
        settings->setMainPanelSize(thumbSizeLarge);
    } else if(index == 3) {
        settings->setMainPanelSize(thumbSizeVeryLarge);
    } else if(index == 4) {
        settings->setMainPanelSize(thumbSizeCustom);
    }

    settings->setMaximumZoom(ui->maxZoomSlider->value());
    settings->setMaxZoomedResolution(ui->maxZoomResSlider->value());
    settings->setThumbnailerThreadCount(ui->thumbnailerThreadsSlider->value());

    applyShortcuts();

    scriptManager->saveScripts();
    actionManager->saveShortcuts();
    emit settingsChanged();
}

void SettingsDialog::applySettingsAndClose() {
    applySettings();
    this->close();
}

void SettingsDialog::populateShortcuts() {
    ui->shortcutsTableWidget->clearContents();
    ui->shortcutsTableWidget->setRowCount(0);
    const QMap<QString, QString> shortcuts = actionManager->allShortcuts();
    QMapIterator<QString, QString> i(shortcuts);
    while(i.hasNext()) {
        i.next();
        addShortcutToTable(i.value(), i.key());
    }
}
//------------------------------------------------------------------------------
void SettingsDialog::populateScripts() {
    ui->scriptsListWidget->clear();
    const QMap<QString, Script> scripts = scriptManager->allScripts();
    QMapIterator<QString, Script> i(scripts);
    while(i.hasNext()) {
        i.next();
        addScriptToList(i.key());
    }
}

// does not check if the shortcut already there
void SettingsDialog::addScriptToList(const QString &name) {
    if(name.isEmpty())
        return;

    QListWidget *list = ui->scriptsListWidget;
    QListWidgetItem *nameItem = new QListWidgetItem(name);
    nameItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    list->insertItem(ui->scriptsListWidget->count(), nameItem);
    list->sortItems(Qt::AscendingOrder);
}

void SettingsDialog::addScript() {
    ScriptEditorDialog w;
    if(w.exec()) {
        if(w.scriptName().isEmpty())
            return;
        scriptManager->addScript(w.scriptName(), w.script());
        populateScripts();
    }
}

void SettingsDialog::editScript() {
    int row = ui->scriptsListWidget->currentRow();
    if(row >= 0) {
        QString name = ui->scriptsListWidget->currentItem()->text();
        editScript(name);
    }
}

void SettingsDialog::editScript(QListWidgetItem* item) {
    if(item) {
        editScript(item->text());
    }
}

void SettingsDialog::editScript(QString name) {
    ScriptEditorDialog w(name, scriptManager->getScript(name));
    if(w.exec()) {
        if(w.scriptName().isEmpty())
            return;
        scriptManager->addScript(w.scriptName(), w.script());
        populateScripts();
    }
}

void SettingsDialog::removeScript() {
    int row = ui->scriptsListWidget->currentRow();
    if(row >= 0) {
        QString scriptName = ui->scriptsListWidget->currentItem()->text();
        delete ui->scriptsListWidget->takeItem(row);
        applyShortcuts();
        actionManager->removeAllShortcuts("s:"+scriptName);
        populateShortcuts();
        scriptManager->removeScript(scriptName);
    }
}
//------------------------------------------------------------------------------
// does not check if the shortcut already there
void SettingsDialog::addShortcutToTable(const QString &action, const QString &shortcut) {
    if(action.isEmpty() || shortcut.isEmpty())
        return;

    ui->shortcutsTableWidget->setRowCount(ui->shortcutsTableWidget->rowCount() + 1);
    QTableWidgetItem *actionItem = new QTableWidgetItem(action);
    actionItem->setTextAlignment(Qt::AlignCenter);
    ui->shortcutsTableWidget->setItem(ui->shortcutsTableWidget->rowCount() - 1, 0, actionItem);
    QTableWidgetItem *shortcutItem = new QTableWidgetItem(shortcut);
    shortcutItem->setTextAlignment(Qt::AlignCenter);
    ui->shortcutsTableWidget->setItem(ui->shortcutsTableWidget->rowCount() - 1, 1, shortcutItem);
    // EFFICIENCY
    ui->shortcutsTableWidget->sortByColumn(0, Qt::AscendingOrder);
}

void SettingsDialog::addShortcut() {
    ShortcutCreatorDialog w;
    if(w.exec()) {
        for(int i = 0; i < ui->shortcutsTableWidget->rowCount(); i++) {
            if(ui->shortcutsTableWidget->item(i, 1)->text() == w.selectedShortcut()) {
                removeShortcutAt(i);
            }
        }
        addShortcutToTable(w.selectedAction(), w.selectedShortcut());
    }
}

void SettingsDialog::removeShortcutAt(int row) {
    if(row > 0 && row >= ui->shortcutsTableWidget->rowCount())
        return;

    ui->shortcutsTableWidget->removeRow(row);
}

void SettingsDialog::removeShortcut() {
    removeShortcutAt(ui->shortcutsTableWidget->currentRow());
}

void SettingsDialog::applyShortcuts() {
    actionManager->removeAllShortcuts();
    for(int i = 0; i < ui->shortcutsTableWidget->rowCount(); i++) {
        actionManager->addShortcut(ui->shortcutsTableWidget->item(i, 1)->text(),
                                   ui->shortcutsTableWidget->item(i, 0)->text());
    }
}

void SettingsDialog::resetShortcuts() {
    actionManager->resetDefaults();
    populateShortcuts();
}

void SettingsDialog::selectMpvPath() {
    QFileDialog dialog;
    QString file;
    file = dialog.getOpenFileName(this, "Navigate to mpv binary", "", "mpv*");
    if(!file.isEmpty()) {
        ui->mpvLineEdit->setText(file);
    }
}

void SettingsDialog::windowColorDialog() {
    QColorDialog *colorDialog = new QColorDialog(this);
    QColor newColor;
    newColor = colorDialog->getColor(windowColorPalette.color(QPalette::Window),
                                     this,
                                     "Windowed mode background");
    if(newColor.isValid()) {
        windowColorPalette.setColor(QPalette::Window, newColor);
        ui->windowColorLabel->setPalette(windowColorPalette);
    }
    delete colorDialog;
}

void SettingsDialog::fullscreenColorDialog() {
    QColorDialog *colorDialog = new QColorDialog(this);
    QColor newColor;
    newColor = colorDialog->getColor(fullscreenColorPalette.color(QPalette::Window),
                                     this,
                                     "Fullscreen mode background");
    if(newColor.isValid()) {
        fullscreenColorPalette.setColor(QPalette::Window, newColor);
        ui->fullscreenColorLabel->setPalette(fullscreenColorPalette);
    }
    delete colorDialog;
}

void SettingsDialog::accentColorDialog() {
    QColorDialog *colorDialog = new QColorDialog(this);
    QColor newColor;
    newColor = colorDialog->getColor(accentLabelPalette.color(QPalette::Window),
                                     this,
                                     "Accent color");
    if(newColor.isValid()) {
        accentLabelPalette.setColor(QPalette::Window, newColor);
        ui->accentColorLabel->setPalette(accentLabelPalette);
    }
    delete colorDialog;
}

void SettingsDialog::onMaxZoomSliderChanged(int value) {
    ui->maxZoomLabel->setText(QString::number(value) + "x");
}

void SettingsDialog::onMaxZoomResolutionSliderChanged(int value) {
    ui->maxZoomResLabel->setText(QString::number(value) + " Mpx");
    ui->maxZoomResInfoLabel->setText("<small><i>Max. memory usage: ~" + QString::number(value * 4) + "MB @ 32bpp</i></small>");
}

void SettingsDialog::onThumbnailerThreadsSliderChanged(int value) {
    ui->thumbnailerThreadsLabel->setText(QString::number(value));
}

void SettingsDialog::onBgOpacitySliderChanged(int value) {
    ui->bgOpacityPercentLabel->setText(QString::number(value) + "%");
}

int SettingsDialog::exec() {
    //this->show();
    //setMinimumSize(sizeHint());
    //resize(sizeHint());
    return QDialog::exec();
}
