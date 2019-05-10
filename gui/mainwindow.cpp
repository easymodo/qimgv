#include "mainwindow.h"

// TODO: nuke this and rewrite

MainWindow::MainWindow(QWidget *parent)
    : OverlayContainerWidget(parent),
      currentDisplay(0),
      desktopWidget(nullptr),
      bgOpacity(1.0f),
      panelEnabled(false),
      panelFullscreenOnly(false),
      activeSidePanel(SIDEPANEL_NONE),
      mainPanel(nullptr)
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

    connect(this, SIGNAL(fullscreenStatusChanged(bool)),
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
    folderView.reset(new FolderView());

    centralWidget.reset(new CentralWidget(docWidget, folderView, this));
    layout.addWidget(centralWidget.get());

    // overlays etc.
    // this order is used while drawing
    infoBarFullscreen = new InfoOverlay(viewerWidget.get());
    controlsOverlay = new ControlsOverlay(docWidget.get());
    saveOverlay = new SaveConfirmOverlay(docWidget.get());

    copyOverlay = new CopyOverlay(this);
    changelogWindow = new ChangelogWindow(this);
    sidePanel = new SidePanel(this);

    // TODO: do something about this spaghetti
    cropOverlay = new CropOverlay(viewerWidget.get());
    cropPanel = new CropPanel(cropOverlay, this);
    connect(cropPanel, SIGNAL(cancel()), this, SLOT(hideCropPanel()));
    connect(cropPanel, SIGNAL(crop(QRect)), this, SLOT(hideCropPanel()));
    connect(cropPanel, SIGNAL(crop(QRect)), this, SIGNAL(cropRequested(QRect)));

    connect(saveOverlay, SIGNAL(saveClicked()), this, SIGNAL(saveRequested()));
    connect(saveOverlay, SIGNAL(saveAsClicked()), this, SIGNAL(saveAsClicked()));
    connect(saveOverlay, SIGNAL(discardClicked()), this, SIGNAL(discardEditsRequested()));
    layout.addWidget(sidePanel);
    connect(copyOverlay, SIGNAL(copyRequested(QString)),
            this, SIGNAL(copyRequested(QString)));
    connect(copyOverlay, SIGNAL(moveRequested(QString)),
            this, SIGNAL(moveRequested(QString)));
    floatingMessage = new FloatingMessage(this);
    thumbnailStrip.reset(new ThumbnailStrip());
    mainPanel = new MainPanel(thumbnailStrip, this);

    connect(viewerWidget.get(), SIGNAL(scalingRequested(QSize)),
            this, SIGNAL(scalingRequested(QSize)));

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

    connect(this, SIGNAL(enableFolderView()),
            mainPanel, SLOT(hide()));
    connect(this, SIGNAL(enableFolderView()),
            this, SLOT(hideCropPanel()));
    connect(this, SIGNAL(enableFolderView()),
            copyOverlay, SLOT(hide()));
    connect(this, SIGNAL(enableFolderView()),
            centralWidget.get(), SLOT(showFolderView()));

    connect(this, SIGNAL(enableDocumentView()),
            centralWidget.get(), SLOT(showDocumentView()));

    connect(this, SIGNAL(setDirectoryPath(QString)),
            folderView.get(), SLOT(setDirectoryPath(QString)));

    connect(this, SIGNAL(toggleFolderView()),
            centralWidget.get(), SLOT(toggleViewMode()));

}

void MainWindow::fitWindow() {
    if(viewerWidget->interactionEnabled()) {
        viewerWidget->fitWindow();
        showMessageFitWindow();
    } else {
        showMessage("Zoom temporary disabled");
    }
}

void MainWindow::fitWidth() {
    if(viewerWidget->interactionEnabled()) {
        viewerWidget->fitWidth();
        showMessageFitWidth();
    } else {
        showMessage("Zoom temporary disabled");
    }
}

void MainWindow::fitOriginal() {
    if(viewerWidget->interactionEnabled()) {
        viewerWidget->fitOriginal();
        showMessageFitOriginal();
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
    infoBarFullscreen->setText("No file opened.");
}

void MainWindow::showImage(std::unique_ptr<QPixmap> pixmap) {
    centralWidget->showDocumentView();
    viewerWidget->showImage(std::move(pixmap));
}

void MainWindow::showAnimation(std::unique_ptr<QMovie> movie) {
    centralWidget->showDocumentView();
    viewerWidget->showAnimation(std::move(movie));
}

void MainWindow::showVideo(Clip *clip) {
    centralWidget->showDocumentView();
    viewerWidget->showVideo(clip);
}

void MainWindow::showContextMenu() {
    viewerWidget->showContextMenu();
}

bool MainWindow::isCropPanelActive() {
    return (activeSidePanel == SIDEPANEL_CROP);
}

void MainWindow::onScalingFinished(std::unique_ptr<QPixmap> scaled) {
    viewerWidget->onScalingFinished(std::move(scaled));
}

void MainWindow::updateCurrentDisplay() {
    currentDisplay = desktopWidget->screenNumber(this);
}

void MainWindow::saveWindowGeometry() {
    #ifdef __linux__
        settings->setWindowGeometry(QRect(pos(), size()));
        //settings->setWindowGeometry(geometry());
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
    const QMimeData *mimeData = event->mimeData();
    // check for our needed mime type, here a file or a list of files
    if(mimeData->hasUrls()) {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        // extract the local paths of the files
        for(int i = 0; i < urlList.size() && i < 32; ++i) {
            pathList.append(urlList.at(i).toLocalFile());
        }
        // try to open first file in the list
        emit opened(pathList.first());
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    OverlayContainerWidget::resizeEvent(event);
    if(activeSidePanel == SIDEPANEL_CROP) {
        cropOverlay->setImageScale(viewerWidget->currentScale());
        cropOverlay->setImageDrawRect(viewerWidget->imageRect());
    }
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
    const QString imagesFilter = settings->supportedFormatsString();
    filePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                                            filePath,
                                            imagesFilter);
    if(!filePath.isEmpty())
        emit saveRequested(filePath);
}

void MainWindow::showOpenDialog() {
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
    emit fullscreenStatusChanged(true);
}

void MainWindow::showWindowed() {
    QWidget::show();
    QWidget::showNormal();
    restoreWindowGeometry();
    //QWidget::activateWindow();
    //QWidget::raise();
    emit fullscreenStatusChanged(false);
}

// passes the side panel all needed info about current image
// TODO: store this info in some kind of singleton? for easy access
void MainWindow::setupSidePanelData() {
    if(activeSidePanel == SIDEPANEL_CROP) {
        cropPanel->setImageRealSize(viewerWidget->sourceSize());
        cropOverlay->setImageDrawRect(viewerWidget->imageRect());
        cropOverlay->setImageScale(viewerWidget->currentScale());
        cropOverlay->setImageRealSize(viewerWidget->sourceSize());
    }
}

void MainWindow::showSaveOverlay() {
    saveOverlay->show();
}

void MainWindow::hideSaveOverlay() {
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
    if(centralWidget->viewMode() == MODE_FOLDERVIEW)
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
        setupSidePanelData();
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

    if(centralWidget->viewMode() == MODE_FOLDERVIEW)
        return;
    if(copyOverlay->operationMode() == OVERLAY_COPY) {
        copyOverlay->isHidden()?copyOverlay->show():copyOverlay->hide();
    } else {
        copyOverlay->setDialogMode(OVERLAY_COPY);
        copyOverlay->show();
    }
}

void MainWindow::triggerMoveOverlay() {
    if(!viewerWidget->isDisplaying())
        return;

    if(centralWidget->viewMode() == MODE_FOLDERVIEW)
        return;
    if(copyOverlay->operationMode() == OVERLAY_MOVE) {
        copyOverlay->isHidden()?copyOverlay->show():copyOverlay->hide();
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
    QString title = fileName;
    if(settings->windowTitleExtendedInfo()) {
        title.prepend("[ " + QString::number(fileIndex + 1) + "/" + QString::number(fileCount) + " ]  ");
        title.append("  -  " + QString::number(imageSize.width()) + " x " + QString::number(imageSize.height()));
        //title.append(" " + QString::number(fileSize / 1024) + " KB");
        // — -
    }
    infoBarFullscreen->setText(title); // temporary
    setWindowTitle(title);
    infoBarWindowed->setInfo(fileIndex, fileCount, fileName, imageSize, fileSize);
}

std::shared_ptr<DirectoryViewWrapper> MainWindow::getFolderView() {
    return folderView->wrapper();
}

std::shared_ptr<DirectoryViewWrapper> MainWindow::getThumbnailPanel() {
    //return thumbnailStrip;
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
    floatingMessage->showMessage(text,  FloatingMessageIcon::ICON_ERROR, 1800);
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
