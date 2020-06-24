#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Preferences — " + qApp->applicationName());

    ui->shortcutsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);   
    ui->aboutAppTextBrowser->viewport()->setAutoFillBackground(false);
    ui->versionLabel->setText("" + QApplication::applicationVersion());
    ui->qtVersionLabel->setText(qVersion());
    ui->appIconLabel->setPixmap(QIcon(":/res/icons/common/logo/app/22.png").pixmap(22,22));
    ui->qtIconLabel->setPixmap(QIcon(":/res/icons/common/logo/3rdparty/qt22.png").pixmap(22,16));

    // fake combobox that acts as a menu button
    // less code than using pushbutton with menu
    // will be replaced with something custom later
    ui->themeSelectorComboBox->setCurrentIndex(-1);

    connect(ui->themeSelectorComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        ui->themeSelectorComboBox->blockSignals(true);
        ui->themeSelectorComboBox->setCurrentIndex(-1);
        ui->themeSelectorComboBox->blockSignals(false);

        switch(index) {
            case 1:  settings->setColorScheme(ThemeStore::colorScheme(COLORS_LIGHT)); break;
            case 0:
            default: settings->setColorScheme(ThemeStore::colorScheme(COLORS_DARK)); break;
        }
        this->readColorScheme();
    });

    connect(ui->useSystemColorsCheckBox, &QCheckBox::toggled, [this](bool useSystemTheme) {
        settings->setUseSystemColorScheme(useSystemTheme);
        this->readColorScheme();
        ui->themeSelectorComboBox->setEnabled(!useSystemTheme);
        ui->colorConfigSubgroup->setEnabled(!useSystemTheme);
    });

    ui->colorSelectorAccent->setDescription("Accent color");
    ui->colorSelectorAccent->setDescription("Windowed mode background");
    ui->colorSelectorAccent->setDescription("Fullscreen mode background");
    ui->colorSelectorAccent->setDescription("FolderView background");
    ui->colorSelectorAccent->setDescription("FolderView top panel");
    ui->colorSelectorAccent->setDescription("Text color");
    ui->colorSelectorAccent->setDescription("Widget background");
    ui->colorSelectorAccent->setDescription("Widget border");
    ui->colorSelectorAccent->setDescription("Overlay background");
    ui->colorSelectorAccent->setDescription("Overlay text");
    ui->colorSelectorAccent->setDescription("Sliders");

#ifndef USE_KDE_BLUR
    ui->blurBackgroundCheckBox->setEnabled(false);
#endif

#ifndef USE_MPV
    ui->videoPlaybackGroup->setEnabled(false);
    ui->novideoInfoLabel->setHidden(false);
#else
    ui->novideoInfoLabel->setHidden(true);
#endif

#ifdef USE_OPENCV
    ui->scalingQualityComboBox->addItem("Bilinear+sharpen (OpenCV)");
    ui->scalingQualityComboBox->addItem("Bicubic (OpenCV)");
    ui->scalingQualityComboBox->addItem("Bicubic+sharpen (OpenCV)");
#endif

    setupSidebar();
    ui->sideBar->setCurrentRow(0);

    connect(this, &SettingsDialog::settingsChanged, settings, &Settings::sendChangeNotification);
    readSettings();
}
//------------------------------------------------------------------------------
SettingsDialog::~SettingsDialog() {
    delete ui;
}
//------------------------------------------------------------------------------
void SettingsDialog::resetToDesktopTheme() {
    settings->setColorScheme(ThemeStore::colorScheme(ColorSchemes::COLORS_SYSTEM));
    this->readColorScheme();
}
//------------------------------------------------------------------------------
void SettingsDialog::setupSidebar() {
    //auto theme = settings->colors().systemtheme;
    QString theme = "dark";
    QListWidget *sideBar = ui->sideBar;
    sideBar->viewport()->setAutoFillBackground(false);
    // General
    sideBar->item(0)->setIcon(QIcon(":res/icons/" + theme + "/settings/general32.png"));
    // Appearance
    sideBar->item(1)->setIcon(QIcon(":res/icons/" + theme + "/settings/appearance32.png"));
    // FolderView
    sideBar->item(2)->setIcon(QIcon(":res/icons/" + theme + "/settings/folderview32.png"));
    // Scaling
    sideBar->item(3)->setIcon(QIcon(":res/icons/" + theme + "/settings/scale32.png"));
    // Controls
    sideBar->item(4)->setIcon(QIcon(":res/icons/" + theme + "/settings/shortcuts32.png"));
    // Scripts
    sideBar->item(5)->setIcon(QIcon(":res/icons/" + theme + "/settings/terminal32.png"));
    // Advanced
    sideBar->item(6)->setIcon(QIcon(":res/icons/" + theme + "/settings/advanced32.png"));
    // About
    sideBar->item(7)->setIcon(QIcon(":res/icons/" + theme + "/settings/about32.png"));

#ifdef _WIN32
    // Not implemented on windows. Not sure if will ever be. I don't really care.
    sideBar->item(5)->setHidden(true);
#endif
}
//------------------------------------------------------------------------------
void SettingsDialog::readSettings() {
    ui->infiniteScrollingCheckBox->setChecked(settings->infiniteScrolling());
    ui->videoPlaybackCheckBox->setChecked(settings->videoPlayback());
    ui->videoPlaybackGroupContents->setEnabled(settings->videoPlayback());
    ui->playSoundsCheckBox->setChecked(settings->playVideoSounds());
    ui->enablePanelCheckBox->setChecked(settings->panelEnabled());
    ui->thumbnailPanelGroupContents->setEnabled(settings->panelEnabled());
    ui->panelFullscreenOnlyCheckBox->setChecked(settings->panelFullscreenOnly());
    ui->squareThumbnailsCheckBox->setChecked(settings->squareThumbnails());
    ui->transparencyGridCheckBox->setChecked(settings->transparencyGrid());
    ui->enableSmoothScrollCheckBox->setChecked(settings->enableSmoothScroll());
    ui->usePreloaderCheckBox->setChecked(settings->usePreloader());
    ui->useThumbnailCacheCheckBox->setChecked(settings->useThumbnailCache());
    ui->smoothUpscalingCheckBox->setChecked(settings->smoothUpscaling());
    ui->expandImageCheckBox->setChecked(settings->expandImage());
    ui->expandImagesGroupContents->setEnabled(settings->expandImage());
    ui->smoothAnimatedImagesCheckBox->setChecked(settings->smoothAnimatedImages());
    ui->bgOpacitySlider->setValue(static_cast<int>(settings->backgroundOpacity() * 100));
    ui->blurBackgroundCheckBox->setChecked(settings->blurBackground());
    ui->sortingComboBox->setCurrentIndex(settings->sortingMode());
    if(settings->zoomIndicatorMode() == INDICATOR_ENABLED)
        ui->zoomIndicatorOn->setChecked(true);
    else if(settings->zoomIndicatorMode() == INDICATOR_AUTO)
        ui->zoomIndicatorAuto->setChecked(true);
    else
        ui->zoomIndicatorOff->setChecked(true);
    ui->showInfoBarFullscreen->setChecked(settings->infoBarFullscreen());
    ui->showInfoBarWindowed->setChecked(settings->infoBarWindowed());
    ui->showExtendedInfoTitle->setChecked(settings->windowTitleExtendedInfo());
    ui->cursorAutohideCheckBox->setChecked(settings->cursorAutohide());
    ui->keepFitModeCheckBox->setChecked(settings->keepFitMode());
    ui->useOpenGLCheckBox->setChecked(settings->useOpenGL());
    ui->focusPointIn1to1ModeComboBox->setCurrentIndex(settings->focusPointIn1to1Mode());
    ui->slideshowIntervalSpinBox->setValue(settings->slideshowInterval());
    ui->imageScrollingComboBox->setCurrentIndex(settings->imageScrolling());

    if(settings->defaultViewMode() == MODE_FOLDERVIEW)
        ui->startInFolderViewCheckBox->setChecked(true);
    else
        ui->startInFolderViewCheckBox->setChecked(false);

    ui->mpvLineEdit->setText(settings->mpvBinary());

    ui->zoomStepSlider->setValue(static_cast<int>(settings->zoomStep() * 10));
    onZoomStepSliderChanged(ui->zoomStepSlider->value());

    ui->JPEGQualitySlider->setValue(settings->JPEGSaveQuality());
    onJPEGQualitySliderChanged(ui->JPEGQualitySlider->value());

    ui->expandLimitSlider->setValue(settings->expandLimit());
    onExpandLimitSliderChanged(ui->expandLimitSlider->value());

    // thumbnailer threads
    ui->thumbnailerThreadsSlider->setValue(settings->thumbnailerThreadCount());
    onThumbnailerThreadsSliderChanged(ui->thumbnailerThreadsSlider->value());

    // ##### fit mode #####
    if(settings->imageFitMode() == FIT_WINDOW)
        ui->fitModeWindow->setChecked(true);
    else if(settings->imageFitMode() == FIT_WIDTH)
        ui->fitModeWidth->setChecked(true);
    else
        ui->fitMode1to1->setChecked(true);

    // ##### UI #####
    ui->scalingQualityComboBox->setCurrentIndex(settings->scalingFilter());
    ui->fullscreenCheckBox->setChecked(settings->fullscreenMode());
    if(settings->panelPosition() == PANEL_TOP)
        ui->panelTop->setChecked(true);
    else
        ui->panelBottom->setChecked(true);
    // reduce by 10x to have nice granular control in qslider
    ui->panelSizeSlider->setValue(settings->mainPanelSize() / 10);

    ui->useSystemColorsCheckBox->setChecked(settings->useSystemColorScheme());
    ui->themeSelectorComboBox->setEnabled(!settings->useSystemColorScheme());
    ui->colorConfigSubgroup->setEnabled(!settings->useSystemColorScheme());

    readColorScheme();
    readShortcuts();
    readScripts();
}
//------------------------------------------------------------------------------
void SettingsDialog::saveSettings() {
    // wait for all background stuff to finish
    if(QThreadPool::globalInstance()->activeThreadCount()) {
        QThreadPool::globalInstance()->waitForDone();
    }

    settings->setInfiniteScrolling(ui->infiniteScrollingCheckBox->isChecked());
    settings->setFullscreenMode(ui->fullscreenCheckBox->isChecked());
    if(ui->fitModeWindow->isChecked())
        settings->setImageFitMode(FIT_WINDOW);
    else if(ui->fitModeWidth->isChecked())
        settings->setImageFitMode(FIT_WIDTH);
    else
        settings->setImageFitMode(FIT_ORIGINAL);
    settings->setVideoPlayback(ui->videoPlaybackCheckBox->isChecked());
    settings->setPlayVideoSounds(ui->playSoundsCheckBox->isChecked());
    settings->setPanelEnabled(ui->enablePanelCheckBox->isChecked());
    settings->setPanelFullscreenOnly(ui->panelFullscreenOnlyCheckBox->isChecked());
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
    if(ui->zoomIndicatorOn->isChecked())
        settings->setZoomIndicatorMode(INDICATOR_ENABLED);
    else if(ui->zoomIndicatorAuto->isChecked())
        settings->setZoomIndicatorMode(INDICATOR_AUTO);
    else
        settings->setZoomIndicatorMode(INDICATOR_DISABLED);
    settings->setInfoBarFullscreen(ui->showInfoBarFullscreen->isChecked());
    settings->setInfoBarWindowed(ui->showInfoBarWindowed->isChecked());
    settings->setWindowTitleExtendedInfo(ui->showExtendedInfoTitle->isChecked());
    settings->setCursorAutohide(ui->cursorAutohideCheckBox->isChecked());
    settings->setKeepFitMode(ui->keepFitModeCheckBox->isChecked());
    settings->setUseOpenGL(ui->useOpenGLCheckBox->isChecked());
    settings->setFocusPointIn1to1Mode(static_cast<ImageFocusPoint>(ui->focusPointIn1to1ModeComboBox->currentIndex()));
    settings->setSlideshowInterval(ui->slideshowIntervalSpinBox->value());

    if(ui->startInFolderViewCheckBox->isChecked())
        settings->setDefaultViewMode(MODE_FOLDERVIEW);
    else
        settings->setDefaultViewMode(MODE_DOCUMENT);

    settings->setMpvBinary(ui->mpvLineEdit->text());

    settings->setScalingFilter(static_cast<ScalingFilter>(ui->scalingQualityComboBox->currentIndex()));

    settings->setImageScrolling(static_cast<ImageScrolling>(ui->imageScrollingComboBox->currentIndex()));

    if(ui->panelTop->isChecked())
        settings->setPanelPosition(PANEL_TOP);
    else
        settings->setPanelPosition(PANEL_BOTTOM);

    settings->setMainPanelSize(ui->panelSizeSlider->value() * 10);

    settings->setJPEGSaveQuality(ui->JPEGQualitySlider->value());
    settings->setZoomStep(static_cast<qreal>(ui->zoomStepSlider->value()) / 10);
    settings->setExpandLimit(ui->expandLimitSlider->value());
    settings->setThumbnailerThreadCount(ui->thumbnailerThreadsSlider->value());

    saveColorScheme();
    saveShortcuts();

    scriptManager->saveScripts();
    actionManager->saveShortcuts();
    emit settingsChanged();
}
//------------------------------------------------------------------------------
void SettingsDialog::saveSettingsAndClose() {
    saveSettings();
    this->close();
}
//------------------------------------------------------------------------------
void SettingsDialog::readColorScheme() {
    auto colors = settings->colorScheme();
    ui->colorSelectorAccent->setColor(colors.accent);
    ui->colorSelectorBackground->setColor(colors.background);
    ui->colorSelectorFullscreen->setColor(colors.background_fullscreen);
    ui->colorSelectorFolderview->setColor(colors.folderview);
    ui->colorSelectorFolderviewPanel->setColor(colors.folderview_topbar);
    ui->colorSelectorText->setColor(colors.text);
    ui->colorSelectorWidget->setColor(colors.widget);
    ui->colorSelectorWidgetBorder->setColor(colors.widget_border);
    ui->colorSelectorOverlay->setColor(colors.overlay);
    ui->colorSelectorOverlayText->setColor(colors.overlay_text);
    ui->colorSelectorSlider->setColor(colors.slider_handle);
}
//------------------------------------------------------------------------------
void SettingsDialog::saveColorScheme() {
    BaseColorScheme base;
    base.accent = ui->colorSelectorAccent->color();
    base.background = ui->colorSelectorBackground->color();
    base.background_fullscreen = ui->colorSelectorFullscreen->color();
    base.folderview = ui->colorSelectorFolderview->color();
    base.folderview_topbar = ui->colorSelectorFolderviewPanel->color();
    base.text = ui->colorSelectorText->color();
    base.widget = ui->colorSelectorWidget->color();
    base.widget_border = ui->colorSelectorWidgetBorder->color();
    base.overlay = ui->colorSelectorOverlay->color();
    base.overlay_text = ui->colorSelectorOverlayText->color();
    base.slider_handle = ui->colorSelectorSlider->color();
    settings->setColorScheme(ColorScheme(base));
}
//------------------------------------------------------------------------------
void SettingsDialog::readShortcuts() {
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
void SettingsDialog::readScripts() {
    ui->scriptsListWidget->clear();
    const QMap<QString, Script> scripts = scriptManager->allScripts();
    QMapIterator<QString, Script> i(scripts);
    while(i.hasNext()) {
        i.next();
        addScriptToList(i.key());
    }
}
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
void SettingsDialog::addScript() {
    ScriptEditorDialog w;
    if(w.exec()) {
        if(w.scriptName().isEmpty())
            return;
        scriptManager->addScript(w.scriptName(), w.script());
        readScripts();
    }
}
//------------------------------------------------------------------------------
void SettingsDialog::editScript() {
    int row = ui->scriptsListWidget->currentRow();
    if(row >= 0) {
        QString name = ui->scriptsListWidget->currentItem()->text();
        editScript(name);
    }
}
//------------------------------------------------------------------------------
void SettingsDialog::editScript(QListWidgetItem* item) {
    if(item) {
        editScript(item->text());
    }
}
//------------------------------------------------------------------------------
void SettingsDialog::editScript(QString name) {
    ScriptEditorDialog w(name, scriptManager->getScript(name));
    if(w.exec()) {
        if(w.scriptName().isEmpty())
            return;
        scriptManager->addScript(w.scriptName(), w.script());
        readScripts();
    }
}
//------------------------------------------------------------------------------
void SettingsDialog::removeScript() {
    int row = ui->scriptsListWidget->currentRow();
    if(row >= 0) {
        QString scriptName = ui->scriptsListWidget->currentItem()->text();
        delete ui->scriptsListWidget->takeItem(row);
        saveShortcuts();
        actionManager->removeAllShortcuts("s:"+scriptName);
        readShortcuts();
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
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
void SettingsDialog::removeShortcutAt(int row) {
    if(row > 0 && row >= ui->shortcutsTableWidget->rowCount())
        return;

    ui->shortcutsTableWidget->removeRow(row);
}
//------------------------------------------------------------------------------
void SettingsDialog::removeShortcut() {
    removeShortcutAt(ui->shortcutsTableWidget->currentRow());
}
//------------------------------------------------------------------------------
void SettingsDialog::saveShortcuts() {
    actionManager->removeAllShortcuts();
    for(int i = 0; i < ui->shortcutsTableWidget->rowCount(); i++) {
        actionManager->addShortcut(ui->shortcutsTableWidget->item(i, 1)->text(),
                                   ui->shortcutsTableWidget->item(i, 0)->text());
    }
}
//------------------------------------------------------------------------------
void SettingsDialog::resetShortcuts() {
    actionManager->resetDefaults();
    readShortcuts();
}
//------------------------------------------------------------------------------
void SettingsDialog::selectMpvPath() {
    QFileDialog dialog;
    QString file;
    file = dialog.getOpenFileName(this, "Navigate to mpv binary", "", "mpv*");
    if(!file.isEmpty()) {
        ui->mpvLineEdit->setText(file);
    }
}
//------------------------------------------------------------------------------
void SettingsDialog::onExpandLimitSliderChanged(int value) {
    if(value == 0)
        ui->expandLimitLabel->setText("-");
    else
        ui->expandLimitLabel->setText(QString::number(value) + "x");
}
//------------------------------------------------------------------------------
void SettingsDialog::onJPEGQualitySliderChanged(int value) {
    ui->JPEGQualityLabel->setText(QString::number(value) + "%");
}
//------------------------------------------------------------------------------
void SettingsDialog::onZoomStepSliderChanged(int value) {
    ui->zoomStepLabel->setText("0." + QString::number(value) + "x");
}
//------------------------------------------------------------------------------
void SettingsDialog::onThumbnailerThreadsSliderChanged(int value) {
    ui->thumbnailerThreadsLabel->setText(QString::number(value));
}
//------------------------------------------------------------------------------
void SettingsDialog::onBgOpacitySliderChanged(int value) {
    ui->bgOpacityPercentLabel->setText(QString::number(value) + "%");
}
//------------------------------------------------------------------------------
int SettingsDialog::exec() {
    return QDialog::exec();
}
