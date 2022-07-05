#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Preferences — ") + qApp->applicationName());

    ui->shortcutsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);   
    ui->aboutAppTextBrowser->viewport()->setAutoFillBackground(false);
    ui->versionLabel->setText("" + QApplication::applicationVersion());
    ui->qtVersionLabel->setText(qVersion());
    ui->appIconLabel->setPixmap(QIcon(":/res/icons/common/logo/app/22.png").pixmap(22,22));
    ui->qtIconLabel->setPixmap(QIcon(":/res/icons/common/logo/3rdparty/qt22.png").pixmap(22,16));

    // fake combobox that acts as a menu button
    // less code than using pushbutton with menu
    // will be replaced with something custom later
    connect(ui->themeSelectorComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        ui->themeSelectorComboBox->blockSignals(true);
        ui->themeSelectorComboBox->setCurrentIndex(index);
        ui->themeSelectorComboBox->blockSignals(false);
        switch(index) {
            case 0: setColorScheme(ThemeStore::colorScheme(COLORS_BLACK));    settings->setColorTid(COLORS_BLACK);    break;
            case 1: setColorScheme(ThemeStore::colorScheme(COLORS_DARK));     settings->setColorTid(COLORS_DARK);     break;
            case 2: setColorScheme(ThemeStore::colorScheme(COLORS_DARKBLUE)); settings->setColorTid(COLORS_DARKBLUE); break;
            case 3: setColorScheme(ThemeStore::colorScheme(COLORS_LIGHT));    settings->setColorTid(COLORS_LIGHT);    break;
        }
    });

    connect(ui->useSystemColorsCheckBox, &QCheckBox::toggled, [this](bool useSystemTheme) {
        if(useSystemTheme) {
            ui->themeSelectorComboBox->setCurrentIndex(-1);
            setColorScheme(ThemeStore::colorScheme(COLORS_SYSTEM));
            settings->setColorTid(COLORS_SYSTEM);
        }
        else {
            readColorScheme();
            settings->setColorTid(COLORS_CUSTOMIZED);
        }
        ui->themeSelectorComboBox->setEnabled(!useSystemTheme);
        ui->colorConfigSubgroup->setEnabled(!useSystemTheme);
        ui->modifySystemSchemeLabel->setVisible(useSystemTheme);
    });

    connect(ui->modifySystemSchemeLabel, &ClickableLabel::clicked, [this]() {
        ui->useSystemColorsCheckBox->setChecked(false);
        setColorScheme(ThemeStore::colorScheme(COLORS_CUSTOMIZED));
        settings->setColorTid(COLORS_CUSTOMIZED);
    });

    ui->colorSelectorAccent->setDescription(tr("Accent color"));
    ui->colorSelectorBackground->setDescription(tr("Windowed mode background"));
    ui->colorSelectorFullscreen->setDescription(tr("Fullscreen mode background"));
    ui->colorSelectorFolderview->setDescription(tr("FolderView background"));
    ui->colorSelectorFolderviewPanel->setDescription(tr("FolderView top panel"));
    ui->colorSelectorText->setDescription(tr("Text color"));
    ui->colorSelectorWidget->setDescription(tr("Widget background"));
    ui->colorSelectorWidgetBorder->setDescription(tr("Widget border"));
    ui->colorSelectorOverlay->setDescription(tr("Overlay background"));
    ui->colorSelectorOverlayText->setDescription(tr("Overlay text"));
    ui->colorSelectorScrollbar->setDescription(tr("Scrollbars"));

#ifndef USE_KDE_BLUR
    ui->blurBackgroundCheckBox->setEnabled(false);
#endif

#ifndef USE_MPV
    //ui->videoPlaybackGroup->setEnabled(false);
    //ui->novideoInfoLabel->setHidden(false);
#else
    //ui->novideoInfoLabel->setHidden(true);
#endif

#ifdef USE_OPENCV
    ui->scalingQualityComboBox->addItem("Bilinear+sharpen (OpenCV)");
    ui->scalingQualityComboBox->addItem("Bicubic (OpenCV)");
    ui->scalingQualityComboBox->addItem("Bicubic+sharpen (OpenCV)");
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    ui->memoryLimitSpinBox->setHidden(true);
    ui->memoryLimitLabel->setHidden(true);
#endif

    if(!settings->supportedFormats().contains("jxl"))
        ui->animatedJxlCheckBox->hide();

    setupSidebar();
    ui->sideBar->setCurrentRow(0);

    // setup radioBtn groups
    fitModeGrp.addButton(ui->fitModeWindow);
    fitModeGrp.addButton(ui->fitModeWidth);
    fitModeGrp.addButton(ui->fitMode1to1);
    folderEndGrp.addButton(ui->folderEndSwitchFolder);
    folderEndGrp.addButton(ui->folderEndNoAction);
    folderEndGrp.addButton(ui->folderEndLoop);
    zoomIndGrp.addButton(ui->zoomIndicatorAuto);
    zoomIndGrp.addButton(ui->zoomIndicatorOff);
    zoomIndGrp.addButton(ui->zoomIndicatorOn);

    // readable language names
    langs.insert("en_US", "English");
    langs.insert("zh_CN", "简体中文");
    // fill langs combobox, sorted by locale
    ui->langComboBox->addItems(langs.values());
    // insert system language entry manually at the beginning
    langs.insert("system", "System language");
    ui->langComboBox->insertItem(0, "System language");

    connect(this, &SettingsDialog::settingsChanged, settings, &Settings::sendChangeNotification);
    readSettings();

    adjustSizeToContents();
}
//------------------------------------------------------------------------------
SettingsDialog::~SettingsDialog() {
    delete ui;
}
//------------------------------------------------------------------------------
// an attempt to force minimum width to fit contents
void SettingsDialog::adjustSizeToContents() {
    // general tab
    ui->gridLayout_15->activate();
    ui->scrollAreaWidgetContents->layout()->activate();
    ui->scrollArea->setMinimumWidth(ui->scrollAreaWidgetContents->minimumSizeHint().width());
    // container
    ui->stackedWidget->layout()->activate();
    this->setMinimumWidth(sizeHint().width() + 22);

    //qDebug() << "window:" << this->sizeHint() << this->minimumSizeHint() << this->size();
    //qDebug() << "stackedwidget:" << ui->stackedWidget->sizeHint() << ui->stackedWidget->minimumSizeHint() << ui->stackedWidget->size();
    //qDebug() << "scrollarea:" << ui->scrollArea->sizeHint() << ui->scrollArea->minimumSizeHint() << ui->scrollArea->size();
    //qDebug() << "scrollareawidget:" << ui->scrollAreaWidgetContents->sizeHint() << ui->scrollAreaWidgetContents->minimumSizeHint() << ui->scrollAreaWidgetContents->size();
    //qDebug() << "grid" << ui->gridLayout_15->sizeHint();
    //qDebug() << "wtf" << ui->startInFolderViewCheckBox->sizeHint() << ui->startInFolderViewCheckBox->minimumSizeHint();
}
//------------------------------------------------------------------------------
void SettingsDialog::resetToDesktopTheme() {
    settings->setColorScheme(ThemeStore::colorScheme(ColorSchemes::COLORS_SYSTEM));
    this->readColorScheme();
}
//------------------------------------------------------------------------------
void SettingsDialog::setupSidebar() {
    QString theme;
    QPalette p;
    if(p.base().color().valueF() <= 0.45f)
        theme = "light";
    else
        theme = "dark";
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

    // empty for now
    sideBar->item(2)->setHidden(true);
}
//------------------------------------------------------------------------------
void SettingsDialog::readSettings() {
    ui->loopSlideshowCheckBox->setChecked(settings->loopSlideshow());
    ui->videoPlaybackCheckBox->setChecked(settings->videoPlayback());
    //ui->videoPlaybackGroupContents->setEnabled(settings->videoPlayback());
    ui->playSoundsCheckBox->setChecked(settings->playVideoSounds());
    ui->enablePanelCheckBox->setChecked(settings->panelEnabled());
    //ui->thumbnailPanelGroupContents->setEnabled(settings->panelEnabled());
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
    ui->confirmDeleteCheckBox->setChecked(settings->confirmDelete());
    ui->confirmTrashCheckBox->setChecked(settings->confirmTrash());
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
    ui->saveOverlayCheckBox->setChecked(settings->showSaveOverlay());
    ui->unloadThumbsCheckBox->setChecked(settings->unloadThumbs());
    ui->thumbStyleComboBox->setCurrentIndex(settings->thumbPanelStyle());
    ui->animatedJxlCheckBox->setChecked(settings->jxlAnimation());
    ui->absoluteStepCheckBox->setChecked(settings->absoluteZoomStep());
    ui->autoResizeWindowCheckBox->setChecked(settings->autoResizeWindow());
    ui->panelCenterSelectionCheckBox->setChecked(settings->panelCenterSelection());

    if(settings->defaultViewMode() == MODE_FOLDERVIEW)
        ui->startInFolderViewCheckBox->setChecked(true);
    else
        ui->startInFolderViewCheckBox->setChecked(false);

    if(settings->folderEndAction() == FOLDER_END_NO_ACTION)
        ui->folderEndNoAction->setChecked(true);
    else if(settings->folderEndAction() == FOLDER_END_LOOP)
        ui->folderEndLoop->setChecked(true);
    else
        ui->folderEndSwitchFolder->setChecked(true);

    ui->mpvLineEdit->setText(settings->mpvBinary());

    ui->zoomStepSlider->setValue(static_cast<int>(settings->zoomStep() * 100.f));
    onZoomStepSliderChanged(ui->zoomStepSlider->value());

    ui->autoResizeLimitSlider->setValue(static_cast<int>(settings->autoResizeLimit() / 5.f));
    onAutoResizeLimitSliderChanged(ui->autoResizeLimitSlider->value());

    ui->JPEGQualitySlider->setValue(settings->JPEGSaveQuality());
    onJPEGQualitySliderChanged(ui->JPEGQualitySlider->value());

    ui->expandLimitSlider->setValue(settings->expandLimit());
    onExpandLimitSliderChanged(ui->expandLimitSlider->value());

    // thumbnailer threads
    ui->thumbnailerThreadsSlider->setValue(settings->thumbnailerThreadCount());
    onThumbnailerThreadsSliderChanged(ui->thumbnailerThreadsSlider->value());

    ui->memoryLimitSpinBox->setValue(settings->memoryAllocationLimit());

    // language
    QString langName = langs.value(settings->language());
    if(langName.isEmpty() || ui->langComboBox->findText(langName) == -1)
        ui->langComboBox->setCurrentText("en_US");
    else
        ui->langComboBox->setCurrentText(langName);

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
    ui->pinPanelCheckBox->setChecked(settings->panelPinned());
    ui->panelPositionComboBox->setCurrentIndex(settings->panelPosition());

    // reduce by 10x to have nice granular control in qslider
    ui->panelSizeSlider->setValue(settings->panelPreviewsSize() / 10);

    ui->useSystemColorsCheckBox->setChecked(settings->useSystemColorScheme());
    ui->modifySystemSchemeLabel->setVisible(settings->useSystemColorScheme());
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

    settings->setLoopSlideshow(ui->loopSlideshowCheckBox->isChecked());
    settings->setFullscreenMode(ui->fullscreenCheckBox->isChecked());
    if(ui->fitModeWindow->isChecked())
        settings->setImageFitMode(FIT_WINDOW);
    else if(ui->fitModeWidth->isChecked())
        settings->setImageFitMode(FIT_WIDTH);
    else
        settings->setImageFitMode(FIT_ORIGINAL);

    settings->setLanguage(langs.key(ui->langComboBox->currentText()));

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

    settings->setBackgroundOpacity(static_cast<qreal>(ui->bgOpacitySlider->value()) / 100.f);
    settings->setBlurBackground(ui->blurBackgroundCheckBox->isChecked());
    settings->setSortingMode(static_cast<SortingMode>(ui->sortingComboBox->currentIndex()));
    settings->setConfirmDelete(ui->confirmDeleteCheckBox->isChecked());
    settings->setConfirmTrash(ui->confirmTrashCheckBox->isChecked());
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

    if(ui->folderEndNoAction->isChecked())
        settings->setFolderEndAction(FOLDER_END_NO_ACTION);
    else if(ui->folderEndLoop->isChecked())
        settings->setFolderEndAction(FOLDER_END_LOOP);
    else
        settings->setFolderEndAction(FOLDER_END_GOTO_ADJACENT);

    settings->setMpvBinary(ui->mpvLineEdit->text());
    settings->setScalingFilter(static_cast<ScalingFilter>(ui->scalingQualityComboBox->currentIndex()));
    settings->setImageScrolling(static_cast<ImageScrolling>(ui->imageScrollingComboBox->currentIndex()));
    settings->setShowSaveOverlay(ui->saveOverlayCheckBox->isChecked());
    settings->setUnloadThumbs(ui->unloadThumbsCheckBox->isChecked());
    settings->setThumbPanelStyle(static_cast<ThumbPanelStyle>(ui->thumbStyleComboBox->currentIndex()));
    settings->setJxlAnimation(ui->animatedJxlCheckBox->isChecked());
    settings->setAbsoluteZoomStep(ui->absoluteStepCheckBox->isChecked());
    settings->setAutoResizeWindow(ui->autoResizeWindowCheckBox->isChecked());
    settings->setPanelCenterSelection(ui->panelCenterSelectionCheckBox->isChecked());

    settings->setPanelPinned(ui->pinPanelCheckBox->isChecked());
    int panelPos = ui->panelPositionComboBox->currentIndex();
    settings->setPanelPosition(static_cast<PanelPosition>(panelPos));

    settings->setPanelPreviewsSize(ui->panelSizeSlider->value() * 10);

    settings->setJPEGSaveQuality(ui->JPEGQualitySlider->value());
    settings->setZoomStep(static_cast<qreal>(ui->zoomStepSlider->value() / 100.f));
    settings->setAutoResizeLimit(ui->autoResizeLimitSlider->value() * 5);
    settings->setExpandLimit(ui->expandLimitSlider->value());
    settings->setThumbnailerThreadCount(ui->thumbnailerThreadsSlider->value());
    settings->setMemoryAllocationLimit(ui->memoryLimitSpinBox->value());

    settings->setUseSystemColorScheme(ui->useSystemColorsCheckBox->isChecked());

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
    setColorScheme(colors);
}

void SettingsDialog::setColorScheme(ColorScheme colors) {
    switch (colors.tid) {
        case COLORS_LIGHT: ui->themeSelectorComboBox->setCurrentIndex(3);   break;
        case COLORS_BLACK: ui->themeSelectorComboBox->setCurrentIndex(0);   break;
        case COLORS_DARK: ui->themeSelectorComboBox->setCurrentIndex(1);    break;
        case COLORS_DARKBLUE: ui->themeSelectorComboBox->setCurrentIndex(2);break;
        default: ui->themeSelectorComboBox->setCurrentIndex(-1);            break;
    }
    ui->colorSelectorAccent->setColor(colors.accent);
    ui->colorSelectorBackground->setColor(colors.background);
    ui->colorSelectorFullscreen->setColor(colors.background_fullscreen);
    ui->colorSelectorFolderview->setColor(colors.folderview);
    ui->colorSelectorFolderviewPanel->setColor(colors.folderview_topbar);
    ui->colorSelectorText->setColor(colors.text);
    ui->colorSelectorIcons->setColor(colors.icons);
    ui->colorSelectorWidget->setColor(colors.widget);
    ui->colorSelectorWidgetBorder->setColor(colors.widget_border);
    ui->colorSelectorOverlay->setColor(colors.overlay);
    ui->colorSelectorOverlayText->setColor(colors.overlay_text);
    ui->colorSelectorScrollbar->setColor(colors.scrollbar);
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
    base.icons = ui->colorSelectorIcons->color();
    base.widget = ui->colorSelectorWidget->color();
    base.widget_border = ui->colorSelectorWidgetBorder->color();
    base.overlay = ui->colorSelectorOverlay->color();
    base.overlay_text = ui->colorSelectorOverlayText->color();
    base.scrollbar = ui->colorSelectorScrollbar->color();
    base.tid = settings->colorScheme().tid;
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
    if(!w.exec())
        return;
    for(int i = 0; i < ui->shortcutsTableWidget->rowCount(); i++) {
        if(ui->shortcutsTableWidget->item(i, 1)->text() == w.selectedShortcut())
            removeShortcutAt(i);
    }
    addShortcutToTable(w.selectedAction(), w.selectedShortcut());
    // select
    auto items = ui->shortcutsTableWidget->findItems(w.selectedShortcut(), Qt::MatchExactly);
    if(items.count()) {
        int newRow = ui->shortcutsTableWidget->row(items.at(0));
        ui->shortcutsTableWidget->selectRow(newRow);
    }
}
//------------------------------------------------------------------------------
void SettingsDialog::removeShortcutAt(int row) {
    if(row > 0 && row >= ui->shortcutsTableWidget->rowCount())
        return;
    ui->shortcutsTableWidget->removeRow(row);
}
//------------------------------------------------------------------------------
void SettingsDialog::editShortcut(int row) {
    if(row >= 0) {
        ShortcutCreatorDialog w;
        w.setWindowTitle(tr("Edit shortcut"));
        w.setAction(ui->shortcutsTableWidget->item(row, 0)->text());
        w.setShortcut(ui->shortcutsTableWidget->item(row, 1)->text());
        if(!w.exec())
            return;
        // remove itself
        removeShortcutAt(row);
        // remove anything we are replacing
        for(int i = 0; i < ui->shortcutsTableWidget->rowCount(); i++) {
            if(ui->shortcutsTableWidget->item(i, 1)->text() == w.selectedShortcut())
                removeShortcutAt(i);
        }
        // re-add
        addShortcutToTable(w.selectedAction(), w.selectedShortcut());
        // re-select
        auto items = ui->shortcutsTableWidget->findItems(w.selectedShortcut(), Qt::MatchExactly);
        if(items.count()) {
            int newRow = ui->shortcutsTableWidget->row(items.at(0));
            ui->shortcutsTableWidget->selectRow(newRow);
        }
    }
}
//------------------------------------------------------------------------------
void SettingsDialog::editShortcut() {
    editShortcut(ui->shortcutsTableWidget->currentRow());
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
    file = dialog.getOpenFileName(this, tr("Navigate to mpv binary"), "", "mpv*");
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
    ui->zoomStepLabel->setText(QString::number(value / 100.f, 'f', 2) + "x");
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
void SettingsDialog::onAutoResizeLimitSliderChanged(int value) {
    ui->autoResizeLimit->setText(QString::number(value * 5.f, 'f', 0) + "%");
}
//------------------------------------------------------------------------------
int SettingsDialog::exec() {
    return QDialog::exec();
}

void SettingsDialog::switchToPage(int number) {
    ui->stackedWidget->setCurrentIndex(number);
    ui->sideBar->setCurrentRow(number);
}
