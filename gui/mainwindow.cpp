#include "mainwindow.h"

MainWindow::MainWindow(ViewerWidget *viewerWidget, QWidget *parent)
    : QWidget(parent),
      currentDisplay(0),
      desktopWidget(NULL),
      panelEnabled(false),
      panelFullscreenOnly(false),
      cropPanelActive(false),
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
    copyDialog = new CopyDialog(this);
    sidePanel = new SidePanel(this);
    cropPanel = new CropPanel(this);
    layout.addWidget(sidePanel);
    connect(cropPanel, SIGNAL(cancel()), sidePanel, SLOT(hide()));
    connect(copyDialog, SIGNAL(copyRequested(QString)),
            this, SIGNAL(copyRequested(QString)));
    connect(copyDialog, SIGNAL(moveRequested(QString)),
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
    return cropPanelActive;
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
    copyDialog->saveSettings();
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
    updateOverlayGeometry();
}

void MainWindow::showDefault() {
    if(!this->isVisible()) {
        if(settings->fullscreenMode())
            showFullScreen();
        else
            showWindowed();
    }
}

void MainWindow::showSaveDialog() {
  /*  const QString imagesFilter = settings->supportedFormatsString();
    QString fileName = core->getCurrentFilePath();
    fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                            fileName,
                                            imagesFilter);
    emit fileSaved(fileName);
    */
}

void MainWindow::showOpenDialog() {
    QFileDialog dialog;
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
    ResizeDialog dialog(initialSize);
    connect(&dialog, SIGNAL(sizeSelected(QSize)),
            this, SIGNAL(resizeRequested(QSize)));
    dialog.exec();
}

void MainWindow::showSettings() {
    SettingsDialog settingsDialog;
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

void MainWindow::toggleCropPanel() {
    if(sidePanel->isHidden()) {
        sidePanel->setWidget(cropPanel);
        sidePanel->show();
        cropPanelActive = true;
    } else {
        sidePanel->hide();
        cropPanelActive = false;
    }
}

void MainWindow::triggerCopyDialog() {
    if(copyDialog->dialogMode() == DIALOG_COPY) {
        copyDialog->isHidden()?copyDialog->show():copyDialog->hide();
    } else {
        copyDialog->setDialogMode(DIALOG_COPY);
        copyDialog->show();
    }
}

void MainWindow::triggerMoveDialog() {
    if(copyDialog->dialogMode() == DIALOG_MOVE) {
        copyDialog->isHidden()?copyDialog->show():copyDialog->hide();
    } else {
        copyDialog->setDialogMode(DIALOG_MOVE);
        copyDialog->show();
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
    floatingMessage->showMessage("", Message::POSITION_RIGHT, Message::ICON_RIGHT_EDGE, 500);
}

void MainWindow::showMessageDirectoryStart() {
    floatingMessage->showMessage("", Message::POSITION_LEFT, Message::ICON_LEFT_EDGE, 500);
}

void MainWindow::showMessageFitWindow() {
    floatingMessage->showMessage("Fit Window", Message::POSITION_BOTTOM, Message::NO_ICON, 1000);
}

void MainWindow::showMessageFitWidth() {
    floatingMessage->showMessage("Fit Width", Message::POSITION_BOTTOM, Message::NO_ICON, 1000);
}

void MainWindow::showMessageFitOriginal() {
    floatingMessage->showMessage("Fit 1:1", Message::POSITION_BOTTOM, Message::NO_ICON, 1000);
}

void MainWindow::showMessage(QString text) {
    floatingMessage->showMessage(text, Message::POSITION_BOTTOM, Message::NO_ICON, 2000);
}

void MainWindow::readSettings() {
    panelPosition = settings->panelPosition();
    panelEnabled = settings->panelEnabled();
    panelFullscreenOnly = settings->panelFullscreenOnly();
    setControlsOverlayEnabled(this->isFullScreen());
    setInfoOverlayEnabled(this->isFullScreen());
    triggerPanelButtons();
}

void MainWindow::updateOverlayGeometry() {
    controlsOverlay->setContainerSize(size());
    infoOverlay->setContainerSize(size());
    floatingMessage->setContainerSize(size());
    mainPanel->setContainerSize(size());
    copyDialog->setContainerSize(size());
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
