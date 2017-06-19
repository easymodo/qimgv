#include "mainwindow.h"

MainWindow::MainWindow(ViewerWidget *viewerWidget, QWidget *parent)
    : QWidget(parent),
      currentDisplay(0),
      desktopWidget(NULL),
      mainPanelEnabled(false),
      sidePanelEnabled(false),
      mainPanel(NULL),
      sidePanel(NULL)
{
    this->setMinimumSize(100, 100);
    layout.setContentsMargins(0,0,0,0);
    this->setLayout(&layout);

    setWindowTitle(QCoreApplication::applicationName() + " " +
                   QCoreApplication::applicationVersion());

    this->setMouseTracking(true);
    this->setAcceptDrops(true);

    setAttribute(Qt::WA_TranslucentBackground);
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
    floatingMessage = new FloatingMessage(this);
    mainPanel = new MainPanel(this);
    sidePanel = new SlideVPanel(this);
}

void MainWindow::setPanelWidget(QWidget *panelWidget) {
    mainPanel->setWidget(panelWidget);
}

bool MainWindow::hasPanelWidget() {
    return mainPanel->hasWidget();
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
        if(mainPanelEnabled && mainPanel->triggerRect().contains(event->pos()) && !mainPanel->triggerRect().contains(lastMouseMovePos))
        {
            mainPanel->show();
        }
        if(sidePanelEnabled && sidePanel->triggerRect().contains(event->pos()) && !sidePanel->triggerRect().contains(lastMouseMovePos))
        {
            sidePanel->show();
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

void MainWindow::closeEvent(QCloseEvent *event) {
    this->hide();
    if(QThreadPool::globalInstance()->activeThreadCount()) {
        QThreadPool::globalInstance()->waitForDone();
    }
    if(!isFullScreen()) {
        saveWindowGeometry();
    }
    saveCurrentDisplay();
    QWidget::closeEvent(event);
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

void MainWindow::show() {
    if(!this->isVisible()) {
        QWidget::show();
        if(settings->fullscreenMode() && !isFullScreen()) {
            this->triggerFullscreen();
        }
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
    connect(&dialog, SIGNAL(fileSelected(QString)), this, SIGNAL(opened(QString)));
    dialog.exec();
}

void MainWindow::showSettings() {
    SettingsDialog settingsDialog;
    settingsDialog.exec();
}

void MainWindow::triggerFullscreen() {
    if(!this->isFullScreen()) {
        //do not save immediately on application start
        if(!this->isHidden())
            saveWindowGeometry();
        //move to target screen
        if(desktopWidget->screenCount() > currentDisplay &&
           currentDisplay != desktopWidget->screenNumber(this))
        {
            this->move(desktopWidget->screenGeometry(currentDisplay).x(),
                       desktopWidget->screenGeometry(currentDisplay).y());
        }
        this->showFullScreen();
        emit fullscreenStatusChanged(true);

    } else {
        this->showNormal();
        restoreWindowGeometry();
        this->activateWindow();
        this->raise();
        emit fullscreenStatusChanged(false);
    }
}

void MainWindow::setInfoString(QString text) {
    infoOverlay->setText(text);
    setWindowTitle(text + " - qimgv");
}

void MainWindow::showMessageDirectoryEnd() {
    floatingMessage->showMessage("", Message::POSITION_RIGHT, Message::ICON_RIGHT_EDGE, 1000);
}

void MainWindow::showMessageDirectoryStart() {
    floatingMessage->showMessage("", Message::POSITION_LEFT, Message::ICON_LEFT_EDGE, 1000);
}

void MainWindow::showMessageFitAll() {
    floatingMessage->showMessage("Fit All", Message::POSITION_BOTTOM, Message::NO_ICON, 1200);
}

void MainWindow::showMessageFitWidth() {
    floatingMessage->showMessage("Fit Width", Message::POSITION_BOTTOM, Message::NO_ICON, 1200);
}

void MainWindow::showMessageFitOriginal() {
    floatingMessage->showMessage("Fit 1:1", Message::POSITION_BOTTOM, Message::NO_ICON, 1200);
}

void MainWindow::readSettings() {
    panelPosition = settings->panelPosition();
    sidePanelPosition = settings->sidePanelPosition();
    mainPanelEnabled = settings->mainPanelEnabled();
    sidePanelEnabled = settings->sidePanelEnabled();

    setControlsOverlayEnabled(this->isFullScreen());
    setInfoOverlayEnabled(this->isFullScreen());
}

void MainWindow::updateOverlayGeometry() {
    controlsOverlay->setContainerSize(size());
    infoOverlay->setContainerSize(size());
    floatingMessage->setContainerSize(size());
    mainPanel->setContainerSize(size());
}

void MainWindow::setControlsOverlayEnabled(bool mode) {
    if(mode && (panelPosition == BOTTOM || !settings->mainPanelEnabled()))
        controlsOverlay->show();
    else
        controlsOverlay->hide();
}

void MainWindow::setInfoOverlayEnabled(bool mode) {
    if(mode)
        infoOverlay->show();
    else
        infoOverlay->hide();
}
