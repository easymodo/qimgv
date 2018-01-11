#include "mainwindow.h"

MainWindow::MainWindow(ViewerWidget *viewerWidget, QWidget *parent)
    : ContainerWidget(parent),
      currentDisplay(0),
      desktopWidget(NULL),
      panelEnabled(false),
      panelFullscreenOnly(false),
      activeSidePanel(SIDEPANEL_NONE),
      mainPanel(NULL)
{
    this->setMinimumSize(400, 300);
    layout.setContentsMargins(0,0,0,0);
    layout.setSpacing(0);

    this->setLayout(&layout);

    setWindowTitle(QCoreApplication::applicationName() + " " +
                   QCoreApplication::applicationVersion());

    this->setMouseTracking(true);
    this->setAcceptDrops(true);

    desktopWidget = QApplication::desktop();
    windowMoveTimer.setSingleShot(true);
    windowMoveTimer.setInterval(150);

    setViewerWidget(viewerWidget);

    setupOverlays();

    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));

    connect(&windowMoveTimer, SIGNAL(timeout()), this, SLOT(updateCurrentDisplay()));

    connect(this, SIGNAL(fullscreenStatusChanged(bool)),
            this, SLOT(setControlsOverlayEnabled(bool)));

    connect(this, SIGNAL(fullscreenStatusChanged(bool)),
            this, SLOT(triggerPanelButtons()));

    connect(this, SIGNAL(fullscreenStatusChanged(bool)),
            this, SLOT(setInfoOverlayEnabled(bool)));

    readSettings();
    currentDisplay = settings->lastDisplay();
    restoreWindowGeometry();
}

// floating panels, info bars etc
// everything that does not go into main layout
void MainWindow::setupOverlays() {
    // this order is used while drawing
    infoOverlay = new InfoOverlay(this);
    controlsOverlay = new ControlsOverlay(this);
    copyOverlay = new CopyOverlay(this);
    sidePanel = new SidePanel(this);
    cropPanel = new CropPanel(this);
    cropOverlay = new CropOverlay(viewerWidget);
    saveOverlay = new SaveConfirmOverlay(viewerWidget);
    connect(saveOverlay, SIGNAL(saveClicked()), this, SIGNAL(saveRequested()));
    connect(saveOverlay, SIGNAL(saveAsClicked()), this, SIGNAL(saveAsClicked()));
    connect(saveOverlay, SIGNAL(discardClicked()), this, SIGNAL(discardEditsRequested()));
    layout.addWidget(sidePanel);
    connect(cropOverlay, SIGNAL(selectionChanged(QRect)),
            cropPanel, SLOT(onSelectionOutsideChange(QRect)));
    connect(cropPanel, SIGNAL(selectionChanged(QRect)),
            cropOverlay, SLOT(onSelectionOutsideChange(QRect)));
    connect(cropPanel, SIGNAL(cancel()), this, SLOT(hideSidePanel()));
    connect(cropPanel, SIGNAL(crop(QRect)), this, SIGNAL(cropRequested(QRect)));
    connect(cropPanel, SIGNAL(crop(QRect)), this, SLOT(hideSidePanel()));
    connect(copyOverlay, SIGNAL(copyRequested(QString)),
            this, SIGNAL(copyRequested(QString)));
    connect(copyOverlay, SIGNAL(moveRequested(QString)),
            this, SIGNAL(moveRequested(QString)));
    floatingMessage = new FloatingMessage(this);
    mainPanel = new MainPanel(this);
}

void MainWindow::setPanelWidget(QWidget *panelWidget) {
    mainPanel->setWidget(panelWidget);
}

bool MainWindow::hasPanelWidget() {
    return mainPanel->hasWidget();
}

bool MainWindow::isCropPanelActive() {
    return (activeSidePanel == SIDEPANEL_CROP);
}

void MainWindow::setViewerWidget(ViewerWidget *viewerWidget) {
    if(viewerWidget) {
        this->viewerWidget = viewerWidget;
        viewerWidget->setParent(this);
        layout.addWidget(viewerWidget);
        viewerWidget->show();
    } else {
        qDebug() << "MainWindow: viewerWidget is null";
    }
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
    if(event->buttons() != Qt::RightButton && event->buttons() != Qt::LeftButton) {
        if(panelEnabled && (isFullScreen() || !panelFullscreenOnly)) {
            if(mainPanel->triggerRect().contains(event->pos()) && !mainPanel->triggerRect().contains(lastMouseMovePos))
            {
                mainPanel->show();
            }
        }
        event->ignore();
    }
    lastMouseMovePos = event->pos();
}

bool MainWindow::event(QEvent *event) {
    if(event->type() == QEvent::MouseMove) {
        return QWidget::event(event);
    }
    if(event->type() == QEvent::Move) {
        windowMoveTimer.start();
        return QWidget::event(event);
    }
    return (actionManager->processEvent(event)) ? true : QWidget::event(event);
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
    ContainerWidget::resizeEvent(event);
    if(activeSidePanel == SIDEPANEL_CROP) {
        cropOverlay->setImageScale(viewerWidget->currentScale());
        cropOverlay->setImageRect(viewerWidget->imageRect());
    }
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
        cropOverlay->setImageRect(viewerWidget->imageRect());
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

void MainWindow::triggerCropPanel() {
    if(activeSidePanel != SIDEPANEL_CROP) {
        showCropPanel();
    } else {
        hideSidePanel();
    }
}

void MainWindow::showCropPanel() {
    if(activeSidePanel != SIDEPANEL_CROP) {
        sidePanel->setWidget(cropPanel);
        sidePanel->show();
        cropOverlay->show();
        activeSidePanel = SIDEPANEL_CROP;
        // reset & lock zoom so CropOverlay won't go crazy
        viewerWidget->fitWindow();
        viewerWidget->disableZoomInteraction();
        // feed the panel current image info
        setupSidePanelData();
    }
}

void MainWindow::hideSidePanel() {
    sidePanel->hide();
    if(activeSidePanel == SIDEPANEL_CROP) {
        cropOverlay->hide();
        viewerWidget->enableZoomInteraction();
    }
    activeSidePanel = SIDEPANEL_NONE;
}

void MainWindow::triggerCopyOverlay() {
    if(copyOverlay->operationMode() == OVERLAY_COPY) {
        copyOverlay->isHidden()?copyOverlay->show():copyOverlay->hide();
    } else {
        copyOverlay->setDialogMode(OVERLAY_COPY);
        copyOverlay->show();
    }
}

void MainWindow::triggerMoveOverlay() {
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

void MainWindow::setInfoString(QString text) {
    infoOverlay->setText(text);
    setWindowTitle(text);
}

void MainWindow::showMessageDirectoryEnd() {
    floatingMessage->showMessage("", FloatingWidgetPosition::RIGHT, FloatingMessageIcon::ICON_RIGHT_EDGE, 700);
}

void MainWindow::showMessageDirectoryStart() {
    floatingMessage->showMessage("", FloatingWidgetPosition::LEFT, FloatingMessageIcon::ICON_LEFT_EDGE, 700);
}

void MainWindow::showMessageFitWindow() {
    floatingMessage->showMessage("Fit Window", FloatingWidgetPosition::BOTTOM, FloatingMessageIcon::NO_ICON, 1000);
}

void MainWindow::showMessageFitWidth() {
    floatingMessage->showMessage("Fit Width", FloatingWidgetPosition::BOTTOM, FloatingMessageIcon::NO_ICON, 1000);
}

void MainWindow::showMessageFitOriginal() {
    floatingMessage->showMessage("Fit 1:1", FloatingWidgetPosition::BOTTOM, FloatingMessageIcon::NO_ICON, 1000);
}

void MainWindow::showMessage(QString text) {
    floatingMessage->showMessage(text, FloatingWidgetPosition::BOTTOM, FloatingMessageIcon::NO_ICON, 2000);
}

void MainWindow::readSettings() {
    panelPosition = settings->panelPosition();
    panelEnabled = settings->panelEnabled();
    panelFullscreenOnly = settings->panelFullscreenOnly();
    setControlsOverlayEnabled(this->isFullScreen());
    setInfoOverlayEnabled(this->isFullScreen());
    triggerPanelButtons();
}

void MainWindow::setControlsOverlayEnabled(bool mode) {
    if(mode && (panelPosition == PANEL_BOTTOM || !settings->panelEnabled()))
        controlsOverlay->show();
    else
        controlsOverlay->hide();
}

// switch some panel buttons on/off depending on
// fullscreen status and other settings
void MainWindow::triggerPanelButtons() {
    if(panelEnabled && panelPosition == PANEL_TOP && isFullScreen())
        mainPanel->setWindowButtonsEnabled(true);
    else
        mainPanel->setWindowButtonsEnabled(false);
}

void MainWindow::setInfoOverlayEnabled(bool mode) {
    if(mode)
        infoOverlay->show();
    else
        infoOverlay->hide();
}
