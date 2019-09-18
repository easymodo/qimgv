#include "mainwindow.h"

// TODO: nuke this and rewrite

MainWindow::MainWindow(QWidget *parent)
    : OverlayContainerWidget(parent),
      currentDisplay(0),
      desktopWidget(nullptr),
      bgOpacity(1.0),
      panelEnabled(false),
      panelFullscreenOnly(false),
      activeSidePanel(SIDEPANEL_NONE),
      mainPanel(nullptr),
      copyOverlay(nullptr),
      saveOverlay(nullptr),
      renameOverlay(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setMinimumSize(400, 300);
    layout.setContentsMargins(0,0,0,0);
    layout.setSpacing(0);

    this->setLayout(&layout);

    setWindowTitle(QCoreApplication::applicationName() + " " +
                   QCoreApplication::applicationVersion());

    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    this->setAccessibleName("mainwindow");
    desktopWidget = QApplication::desktop();
    windowMoveTimer.setSingleShot(true);
    windowMoveTimer.setInterval(150);

    setupUi();

    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));

    connect(&windowMoveTimer, SIGNAL(timeout()),
            this, SLOT(updateCurrentDisplay()));

    connect(this, SIGNAL(fullscreenStateChanged(bool)),
            this, SLOT(adaptToWindowState()));

    readSettings();
    currentDisplay = settings->lastDisplay();
    restoreWindowGeometry();
}

/*                                                                         |-- [ImageViewer]
 *                                   |-- [DocumentWidget]--[ViewerWidget]--|
 * [MainWindow] -- [CentralWidget] --|                                     |-- [VideoPlayer]
 *                                   |-- [FolderView]
 *
 *  (not counting floating widgets)
 *  ViewerWidget exists for input handling reasons (correct overlay hover handling)
 */
void MainWindow::setupUi() {
    viewerWidget.reset(new ViewerWidget());
    infoBarWindowed.reset(new InfoBar());
    docWidget.reset(new DocumentWidget(viewerWidget, infoBarWindowed));

    folderView.reset(new FolderView()); // todo

    centralWidget.reset(new CentralWidget(docWidget, folderView, this));
    layout.addWidget(centralWidget.get());

    // overlays etc.
    // this order is used while drawing
    infoBarFullscreen = new InfoOverlay(viewerWidget.get()); // todo
    controlsOverlay = new ControlsOverlay(docWidget.get()); // todo

    imageInfoOverlay = new ImageInfoOverlay(this);
    changelogWindow = new ChangelogWindow(this);
    sidePanel = new SidePanel(this);

    // TODO: do something about this spaghetti
    cropOverlay = new CropOverlay(viewerWidget.get());
    cropPanel = new CropPanel(cropOverlay, this);
    connect(cropPanel, SIGNAL(cancel()), this, SLOT(hideCropPanel()));
    connect(cropPanel, SIGNAL(crop(QRect)), this, SLOT(hideCropPanel()));
    connect(cropPanel, SIGNAL(crop(QRect)), this, SIGNAL(cropRequested(QRect)));

    layout.addWidget(sidePanel);

    floatingMessage = new FloatingMessage(this);
    thumbnailStrip.reset(new ThumbnailStrip());
    mainPanel = new MainPanel(thumbnailStrip, this);

    connect(viewerWidget.get(), SIGNAL(scalingRequested(QSize, ScalingFilter)),
            this, SIGNAL(scalingRequested(QSize, ScalingFilter)));

    connect(viewerWidget.get(), SIGNAL(draggedOut()),
            this, SIGNAL(draggedOut()));

    //  Core >> MW
    connect(this, SIGNAL(zoomIn()),
            viewerWidget.get(), SIGNAL(zoomIn()));
    connect(this, SIGNAL(zoomOut()),
            viewerWidget.get(), SIGNAL(zoomOut()));
    connect(this, SIGNAL(zoomInCursor()),
            viewerWidget.get(), SIGNAL(zoomInCursor()));
    connect(this, SIGNAL(zoomOutCursor()),
            viewerWidget.get(), SIGNAL(zoomOutCursor()));

    connect(this, SIGNAL(scrollUp()),
            viewerWidget.get(), SIGNAL(scrollUp()));
    connect(this, SIGNAL(scrollDown()),
            viewerWidget.get(), SIGNAL(scrollDown()));
    connect(this, SIGNAL(scrollLeft()),
            viewerWidget.get(), SIGNAL(scrollLeft()));
    connect(this, SIGNAL(scrollRight()),
            viewerWidget.get(), SIGNAL(scrollRight()));
    connect(this, SIGNAL(pauseVideo()),
            viewerWidget.get(), SLOT(pauseVideo()));
    connect(this, SIGNAL(stopPlayback()),
            viewerWidget.get(), SLOT(stopPlayback()));
    connect(this, SIGNAL(seekVideoRight()),
            viewerWidget.get(), SLOT(seekVideoRight()));
    connect(this, SIGNAL(seekVideoLeft()),
            viewerWidget.get(), SLOT(seekVideoLeft()));
    connect(this, SIGNAL(frameStep()),
            viewerWidget.get(), SLOT(frameStep()));
    connect(this, SIGNAL(frameStepBack()),
            viewerWidget.get(), SLOT(frameStepBack()));

    connect(this, SIGNAL(toggleTransparencyGrid()),
            viewerWidget.get(), SIGNAL(toggleTransparencyGrid()));

    connect(this, SIGNAL(enableDocumentView()),
            centralWidget.get(), SLOT(showDocumentView()));

    connect(this, SIGNAL(setDirectoryPath(QString)),
            folderView.get(), SLOT(setDirectoryPath(QString)));

    connect(folderView.get(), SIGNAL(sortingSelected(SortingMode)),
            this, SIGNAL(sortingSelected(SortingMode)));
}

void MainWindow::setupCopyOverlay() {
    copyOverlay = new CopyOverlay(this);
    connect(copyOverlay, SIGNAL(copyRequested(QString)),
            this, SIGNAL(copyRequested(QString)));
    connect(copyOverlay, SIGNAL(moveRequested(QString)),
            this, SIGNAL(moveRequested(QString)));
}

void MainWindow::setupSaveOverlay() {
    saveOverlay = new SaveConfirmOverlay(docWidget.get());
    connect(saveOverlay, SIGNAL(saveClicked()), this, SIGNAL(saveRequested()));
    connect(saveOverlay, SIGNAL(saveAsClicked()), this, SIGNAL(saveAsClicked()));
    connect(saveOverlay, SIGNAL(discardClicked()), this, SIGNAL(discardEditsRequested()));

}

void MainWindow::setupRenameOverlay() {
    renameOverlay = new RenameOverlay(this);
    connect(renameOverlay, &RenameOverlay::renameRequested, this, &MainWindow::renameRequested);
}

void MainWindow::toggleFolderView() {
    hideCropPanel();
    if(copyOverlay)
        copyOverlay->hide();
    if(renameOverlay)
        renameOverlay->hide();
    mainPanel->hide();
    imageInfoOverlay->hide();
    centralWidget->toggleViewMode();
}

void MainWindow::enableFolderView() {
    hideCropPanel();
    if(copyOverlay)
        copyOverlay->hide();
    if(renameOverlay)
        renameOverlay->hide();
    mainPanel->hide();
    imageInfoOverlay->hide();
    centralWidget->showFolderView();
}

void MainWindow::fitWindow() {
    if(viewerWidget->interactionEnabled()) {
        viewerWidget->fitWindow();
        //showMessageFitWindow();
    } else {
        showMessage("Zoom temporary disabled");
    }
}

void MainWindow::fitWidth() {
    if(viewerWidget->interactionEnabled()) {
        viewerWidget->fitWidth();
        //showMessageFitWidth();
    } else {
        showMessage("Zoom temporary disabled");
    }
}

void MainWindow::fitOriginal() {
    if(viewerWidget->interactionEnabled()) {
        viewerWidget->fitOriginal();
        //showMessageFitOriginal();
    } else {
        showMessage("Zoom temporary disabled");
    }
}

// switch between 1:1 and Fit All
// TODO: move to viewerWidget?
void MainWindow::switchFitMode() {
    if(viewerWidget->fitMode() == FIT_WINDOW)
        viewerWidget->setFitMode(FIT_ORIGINAL);
    else
        viewerWidget->setFitMode(FIT_WINDOW);
}

void MainWindow::closeImage() {
    viewerWidget->closeImage();
    //infoBarFullscreen->setText("No file opened.");
}

void MainWindow::showImage(std::unique_ptr<QPixmap> pixmap) {
    centralWidget->showDocumentView();
    viewerWidget->showImage(std::move(pixmap));
    updateCropPanelData();
}

void MainWindow::showAnimation(std::unique_ptr<QMovie> movie) {
    centralWidget->showDocumentView();
    viewerWidget->showAnimation(std::move(movie));
    updateCropPanelData();
}

void MainWindow::showVideo(QString file) {
    centralWidget->showDocumentView();
    viewerWidget->showVideo(file);
}

void MainWindow::showContextMenu() {
    viewerWidget->showContextMenu();
}

void MainWindow::onSortingChanged(SortingMode mode) {
    folderView.get()->onSortingChanged(mode);
    if(centralWidget.get()->currentViewMode() == ViewMode::MODE_DOCUMENT) {
        switch(mode) {
            case SortingMode::SORT_NAME:      showMessage("Sorting: By Name");              break;
            case SortingMode::SORT_NAME_DESC: showMessage("Sorting: By Name (desc.)");      break;
            case SortingMode::SORT_TIME:      showMessage("Sorting: By Time");              break;
            case SortingMode::SORT_TIME_DESC: showMessage("Sorting: By Time (desc.)");      break;
            case SortingMode::SORT_SIZE:      showMessage("Sorting: By File Size");         break;
            case SortingMode::SORT_SIZE_DESC: showMessage("Sorting: By File Size (desc.)"); break;
        }
    }
}

void MainWindow::toggleImageInfoOverlay() {
    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW)
        return;
    if(imageInfoOverlay->isHidden())
        imageInfoOverlay->show();
    else
        imageInfoOverlay->hide();
}

void MainWindow::toggleRenameOverlay() {
    if(!renameOverlay)
        setupRenameOverlay();
    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW)
        return;
    if(renameOverlay->isHidden())
        renameOverlay->show();
    else
        renameOverlay->hide();
}

void MainWindow::toggleScalingFilter() {
    if(viewerWidget->scalingFilter() == FILTER_BILINEAR)
        setFilterNearest();
    else
        setFilterBilinear();
}

void MainWindow::setFilterNearest() {
    showMessage("Filter: nearest", 600);
    viewerWidget->setFilterNearest();
}

void MainWindow::setFilterBilinear() {
    showMessage("Filter: bilinear", 600);
    viewerWidget->setFilterBilinear();
}

bool MainWindow::isCropPanelActive() {
    return (activeSidePanel == SIDEPANEL_CROP);
}

void MainWindow::onScalingFinished(std::unique_ptr<QPixmap> scaled) {
    viewerWidget->onScalingFinished(std::move(scaled));
}

void MainWindow::saveWindowGeometry() {
    #ifdef __linux__
    if(this->isHidden())
        settings->setWindowGeometry(QRect(pos(), size()));
    else
        settings->setWindowGeometry(geometry());
    #else
         settings->setWindowGeometry(QRect(pos(), size()));
    #endif
}

void MainWindow::restoreWindowGeometry() {
    QRect geometry = settings->windowGeometry();
    this->resize(geometry.size());
    this->move(geometry.x(), geometry.y());
    updateCurrentDisplay();
}

void MainWindow::updateCurrentDisplay() {
    currentDisplay = desktopWidget->screenNumber(this);
}

void MainWindow::saveCurrentDisplay() {
    settings->setLastDisplay(desktopWidget->screenNumber(this));
}

//#############################################################
//######################### EVENTS ############################
//#############################################################

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() != Qt::NoButton) {
        lastMouseMovePos = event->pos();
        return;
    }
    // show on hover event
    if(panelEnabled && (isFullScreen() || !panelFullscreenOnly)) {
        if( mainPanel->triggerRect().contains(event->pos()) && !mainPanel->triggerRect().contains(lastMouseMovePos))
        {
            mainPanel->show();
        }
    }
    // fade out on leave event
    if(!mainPanel->isHidden()) {
        // leaveEvent which misfires on HiDPI (rounding error somewhere?)
        // add a few px of buffer area to avoid bugs
        // it still fcks up Fitts law as the buttons are not receiving hover on screen border
        if(!mainPanel->triggerRect().adjusted(-8,-8,8,8).contains(event->pos()))
        {
            mainPanel->hideAnimated();
        }
    }
    lastMouseMovePos = event->pos();
    event->ignore();
}

bool MainWindow::event(QEvent *event) {
    if(event->type() == QEvent::Move)
        windowMoveTimer.start();
    return QWidget::event(event);
    //return (actionManager->processEvent(event)) ? true : QWidget::event(event);
}

// hook up to actionManager
void MainWindow::keyPressEvent(QKeyEvent *event) {
    event->accept();
    actionManager->processEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    event->accept();
    actionManager->processEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    event->accept();
    actionManager->processEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    event->accept();
    actionManager->processEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    event->accept();
    QMouseEvent *fakePressEvent = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), event->button(), event->buttons(), event->modifiers());
    actionManager->processEvent(fakePressEvent);
    actionManager->processEvent(event);
}

void MainWindow::close() {
    this->hide();
    if(!isFullScreen()) {
        saveWindowGeometry();
    }
    saveCurrentDisplay();
    if(copyOverlay)
        copyOverlay->saveSettings();
    QWidget::close();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // catch the close event when user presses X on the window itself
    event->accept();
    actionManager->invokeAction("exit");
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e) {
    if(e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    emit droppedIn(event->mimeData(), event->source());
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    if(activeSidePanel == SIDEPANEL_CROP) {
        cropOverlay->setImageScale(viewerWidget->currentScale());
        cropOverlay->setImageDrawRect(viewerWidget->imageRect());
    }
    OverlayContainerWidget::resizeEvent(event);
}

void MainWindow::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    if(mainPanel)
        mainPanel->hideAnimated();
}

void MainWindow::showDefault() {
    if(!this->isVisible()) {
        if(settings->fullscreenMode())
            showFullScreen();
        else
            showWindowed();
    }
}

void MainWindow::showSaveDialog(QString filePath) {
    if(mainPanel)
        mainPanel->hide();

    const QString imagesFilter = settings->supportedFormatsString();
    filePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                                            filePath,
                                            imagesFilter);
    if(!filePath.isEmpty())
        emit saveRequested(filePath);
}

void MainWindow::showOpenDialog() {
    // Looks like there is a bug in qt with native file dialogs
    // It hangs at exec() when there is a panel visible
    // Works fine otherwise, or with builtin qt dialogs
    // will try to investigate later
    if(mainPanel)
        mainPanel->hide();

    QFileDialog dialog(this);
    QStringList imageFilter;
    imageFilter.append(settings->supportedFormatsString());
    imageFilter.append("All Files (*)");
    QString lastDir = settings->lastDirectory();
    dialog.setDirectory(lastDir);
    dialog.setNameFilters(imageFilter);
    dialog.setWindowTitle("Open image");
    dialog.setWindowModality(Qt::ApplicationModal);
    connect(&dialog, SIGNAL(fileSelected(QString)),
            this, SIGNAL(opened(QString)));

    dialog.exec();
}

void MainWindow::showResizeDialog(QSize initialSize) {
    ResizeDialog dialog(initialSize, this);
    connect(&dialog, SIGNAL(sizeSelected(QSize)),
            this, SIGNAL(resizeRequested(QSize)));
    dialog.exec();
}

void MainWindow::showSettings() {
    if(mainPanel)
        mainPanel->hide();

    SettingsDialog settingsDialog(this);
    settingsDialog.exec();
}

void MainWindow::triggerFullScreen() {
    if(!isFullScreen()) {
        showFullScreen();
    } else {
        showWindowed();
    }
}

void MainWindow::showFullScreen() {
    //do not save immediately on application start
    if(!isHidden())
        saveWindowGeometry();
    //move to target screen
    if(desktopWidget->screenCount() > currentDisplay &&
       currentDisplay != desktopWidget->screenNumber(this))
    {
        this->move(desktopWidget->screenGeometry(currentDisplay).x(),
                   desktopWidget->screenGeometry(currentDisplay).y());
    }
    QWidget::showFullScreen();
    emit fullscreenStateChanged(true);
}

void MainWindow::showWindowed() {
    QWidget::show();
    QWidget::showNormal();
    restoreWindowGeometry();
    //QWidget::activateWindow();
    //QWidget::raise();
    emit fullscreenStateChanged(false);
}

void MainWindow::updateCropPanelData() {
    if(activeSidePanel == SIDEPANEL_CROP) {
        cropPanel->setImageRealSize(viewerWidget->sourceSize());
        cropOverlay->setImageDrawRect(viewerWidget->imageRect());
        cropOverlay->setImageScale(viewerWidget->currentScale());
        cropOverlay->setImageRealSize(viewerWidget->sourceSize());
    }
}

void MainWindow::showSaveOverlay() {
    if(!saveOverlay)
        setupSaveOverlay();
    saveOverlay->show();
}

void MainWindow::hideSaveOverlay() {
    if(!saveOverlay)
        return;
    saveOverlay->hide();
}

void MainWindow::showChangelogWindow() {
    changelogWindow->show();
}

void MainWindow::showChangelogWindow(QString text) {
    changelogWindow->setText(text);
    changelogWindow->show();
}

void MainWindow::triggerCropPanel() {
    if(activeSidePanel != SIDEPANEL_CROP) {
        showCropPanel();
    } else {
        hideCropPanel();
    }
}

void MainWindow::showCropPanel() {
    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW)
        return;

    if(activeSidePanel != SIDEPANEL_CROP) {
        (mainPanel)->hide();
        sidePanel->setWidget(cropPanel);
        sidePanel->show();
        cropOverlay->show();
        activeSidePanel = SIDEPANEL_CROP;
        // reset & lock zoom so CropOverlay won't go crazy
        viewerWidget->fitWindow();
        viewerWidget->disableInteraction();
        // feed the panel current image info
        updateCropPanelData();
    }
}

void MainWindow::hideCropPanel() {
    sidePanel->hide();
    if(activeSidePanel == SIDEPANEL_CROP) {
        cropOverlay->hide();
        viewerWidget->enableInteraction();
    }
    activeSidePanel = SIDEPANEL_NONE;
}

void MainWindow::triggerCopyOverlay() {
    if(!viewerWidget->isDisplaying())
        return;
    if(!copyOverlay)
        setupCopyOverlay();

    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW)
        return;
    if(copyOverlay->operationMode() == OVERLAY_COPY) {
        copyOverlay->isHidden() ? copyOverlay->show() : copyOverlay->hide();
    } else {
        copyOverlay->setDialogMode(OVERLAY_COPY);
        copyOverlay->show();
    }
}

void MainWindow::triggerMoveOverlay() {
    if(!viewerWidget->isDisplaying())
        return;
    if(!copyOverlay)
        setupCopyOverlay();

    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW)
        return;
    if(copyOverlay->operationMode() == OVERLAY_MOVE) {
        copyOverlay->isHidden() ? copyOverlay->show() : copyOverlay->hide();
    } else {
        copyOverlay->setDialogMode(OVERLAY_MOVE);
        copyOverlay->show();
    }
}

// quit fullscreen or exit the program
void MainWindow::closeFullScreenOrExit() {
    if(this->isFullScreen()) {
        this->showWindowed();
    } else {
        actionManager->invokeAction("exit");
    }
}

void MainWindow::setCurrentInfo(int fileIndex, int fileCount, QString fileName, QSize imageSize, int fileSize) {
    if(renameOverlay) // TODO: use some global state for convenience?
        renameOverlay->setName(fileName);
    if(fileName.isEmpty()) {
        setWindowTitle(qApp->applicationName());
        infoBarFullscreen->setInfo("", "No file opened.", "");
        infoBarWindowed->setInfo("", "No file opened.", "");
    } else {
        QString posString;
        if(fileCount)
            posString = "[ " + QString::number(fileIndex + 1) + "/" + QString::number(fileCount) + " ]";
        QString resString;
        if(imageSize.width())
            resString = QString::number(imageSize.width()) + " x " + QString::number(imageSize.height());
        QString sizeString;
        if(fileSize)
            sizeString = QString::number(fileSize / 1024) + " KiB";

        QString windowTitle = fileName;
        if(settings->windowTitleExtendedInfo()) {
            windowTitle.prepend(posString + "  ");
            if(!resString.isEmpty())
                windowTitle.append("  -  " + resString);
            if(!sizeString.isEmpty())
                windowTitle.append("  -  " + sizeString);
        }
        infoBarFullscreen->setInfo(posString, fileName, resString + "  " + sizeString);
        setWindowTitle(windowTitle);
        infoBarWindowed->setInfo(posString, fileName, resString + "  " + sizeString);
    }
}

void MainWindow::setExifInfo(QMap<QString, QString> info) {
    imageInfoOverlay->setExifInfo(info);
}

std::shared_ptr<DirectoryViewWrapper> MainWindow::getFolderView() {
    return folderView->wrapper();
}

std::shared_ptr<DirectoryViewWrapper> MainWindow::getThumbnailPanel() {
    return this->thumbnailStrip->wrapper();
}

void MainWindow::showMessageDirectoryEnd() {
    floatingMessage->showMessage("", FloatingWidgetPosition::RIGHT, FloatingMessageIcon::ICON_RIGHT_EDGE, 400);
}

void MainWindow::showMessageDirectoryStart() {
    floatingMessage->showMessage("", FloatingWidgetPosition::LEFT, FloatingMessageIcon::ICON_LEFT_EDGE, 400);
}

void MainWindow::showMessageFitWindow() {
    floatingMessage->showMessage("Fit Window", FloatingMessageIcon::NO_ICON, 350);
}

void MainWindow::showMessageFitWidth() {
    floatingMessage->showMessage("Fit Width", FloatingMessageIcon::NO_ICON, 350);
}

void MainWindow::showMessageFitOriginal() {
    floatingMessage->showMessage("Fit 1:1", FloatingMessageIcon::NO_ICON, 350);
}

void MainWindow::showMessage(QString text) {
    floatingMessage->showMessage(text,  FloatingMessageIcon::NO_ICON, 1500);
}

void MainWindow::showMessage(QString text, int duration) {
    floatingMessage->showMessage(text, FloatingMessageIcon::NO_ICON, duration);
}

void MainWindow::showMessageSuccess(QString text) {
    floatingMessage->showMessage(text,  FloatingMessageIcon::ICON_SUCCESS, 1500);
}

void MainWindow::showWarning(QString text) {
    floatingMessage->showMessage(text,  FloatingMessageIcon::ICON_WARNING, 1500);
}

void MainWindow::showError(QString text) {
    floatingMessage->showMessage(text,  FloatingMessageIcon::ICON_ERROR, 2500);
}

void MainWindow::readSettings() {
    panelPosition = settings->panelPosition();
    panelEnabled = settings->panelEnabled();
    panelFullscreenOnly = settings->panelFullscreenOnly();
    showInfoBarFullscreen = settings->infoBarFullscreen();
    showInfoBarWindowed = settings->infoBarWindowed();
    adaptToWindowState();
}

void MainWindow::applyWindowedBackground() {
    bgColor = settings->backgroundColor();
    bgOpacity = settings->backgroundOpacity();
#ifdef USE_KDE_BLUR
    if(bgOpacity == 1.0)
        KWindowEffects::enableBlurBehind(winId(), false);
    else
        KWindowEffects::enableBlurBehind(winId(), settings->blurBackground());
#endif
}

void MainWindow::applyFullscreenBackground() {
    bgColor = settings->backgroundColorFullscreen();
    bgOpacity = 1.0;
#ifdef USE_KDE_BLUR
    KWindowEffects::enableBlurBehind(winId(), false);
#endif
}

// changes ui elements according to fullscreen state
void MainWindow::adaptToWindowState() {
    if(panelEnabled)
        mainPanel->hide();
    if(isFullScreen()) { //-------------------------------------- fullscreen ---
        applyFullscreenBackground();
        infoBarWindowed->hide();
        if(showInfoBarFullscreen)
            infoBarFullscreen->show();
        else
            infoBarFullscreen->hide();
        folderView->setExitButtonEnabled(true);
        if(panelEnabled && panelPosition == PANEL_TOP)
            mainPanel->setExitButtonEnabled(true);
        if(panelPosition == PANEL_BOTTOM || !panelEnabled)
            controlsOverlay->show();
        else
            controlsOverlay->hide();
    } else { //------------------------------------------------------ window ---
        applyWindowedBackground();
        infoBarFullscreen->hide();
        if(showInfoBarWindowed)
            infoBarWindowed->show();
        else
            infoBarWindowed->hide();
        controlsOverlay->hide();
        folderView->setExitButtonEnabled(false);
        mainPanel->setExitButtonEnabled(false);
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setOpacity(bgOpacity);
    p.setBrush(QBrush(bgColor));
    p.fillRect(this->rect(), p.brush());
    OverlayContainerWidget::paintEvent(event);
}
