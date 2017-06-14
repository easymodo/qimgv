#include "mainwindow2.h"

MainWindow2::MainWindow2(ViewerWidget *viewerWidget, QWidget *parent)
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
    this->setGeometry(200,130,1300,800);

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
    if(settings->fullscreenMode() && !isFullScreen()) {
        this->triggerFullscreen();
    }
}

// floating panels, info bars etc
// everything that does not go into main layout
void MainWindow2::setupOverlays() {
    // this order is used while drawing
    infoOverlay = new InfoOverlay(this);
    controlsOverlay = new ControlsOverlay(this);
    overlayMessage = new OverlayMessage(this);
    mainPanel = new MainPanel(this);
    sidePanel = new SlideVPanel(this);
}

void MainWindow2::setPanelWidget(QWidget *panelWidget) {
    mainPanel->setWidget(panelWidget);
}

bool MainWindow2::hasPanelWidget() {
    return mainPanel->hasWidget();
}

void MainWindow2::setViewerWidget(ViewerWidget *viewerWidget) {
    if(viewerWidget) {
        this->viewerWidget = viewerWidget;
        viewerWidget->setParent(this);
        layout.addWidget(viewerWidget);
        viewerWidget->show();
    } else {
        qDebug() << "MainWindow: viewerWidget is null";
    }
}

void MainWindow2::updateCurrentDisplay() {
    currentDisplay = desktopWidget->screenNumber(this);
}

void MainWindow2::saveWindowGeometry() {
    #ifdef __linux__
        settings->setWindowGeometry(QRect(pos(), size()));
        //settings->setWindowGeometry(geometry());
    #else
         settings->setWindowGeometry(QRect(pos(), size()));
    #endif
}

void MainWindow2::restoreWindowGeometry() {
    QRect geometry = settings->windowGeometry();
    this->resize(geometry.size());
    this->move(geometry.x(), geometry.y());
    updateCurrentDisplay();
}

void MainWindow2::saveCurrentDisplay() {
    settings->setLastDisplay(desktopWidget->screenNumber(this));
}

//#############################################################
//######################### EVENTS ############################
//#############################################################

void MainWindow2::mouseMoveEvent(QMouseEvent *event) {
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

bool MainWindow2::event(QEvent *event) {
    if(event->type() == QEvent::MouseMove) {
        return QWidget::event(event);
    }
    if(event->type() == QEvent::Move) {
        windowMoveTimer.start();
        return QWidget::event(event);
    }
    return (actionManager->processEvent(event)) ? true : QWidget::event(event);
}

void MainWindow2::closeEvent(QCloseEvent *event) {
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

void MainWindow2::dragEnterEvent(QDragEnterEvent *e) {
    if(e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow2::dropEvent(QDropEvent *event) {
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

void MainWindow2::resizeEvent(QResizeEvent *event) {
    updateOverlayGeometry();
}

void MainWindow2::showSaveDialog() {
  /*  const QString imagesFilter = settings->supportedFormatsString();
    QString fileName = core->getCurrentFilePath();
    fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                            fileName,
                                            imagesFilter);
    emit fileSaved(fileName);
    */
}

void MainWindow2::showOpenDialog() {
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

void MainWindow2::showSettings() {
    SettingsDialog settingsDialog;
    settingsDialog.exec();
}

void MainWindow2::triggerFullscreen() {
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

void MainWindow2::setInfoString(QString text) {
    infoOverlay->setText(text);
    setWindowTitle(text + " - qimgv");
}

void MainWindow2::showMessageDirectoryEnd() {
    overlayMessage->showMessage("end|", MESSAGE_RIGHT, 1000);
}

void MainWindow2::showMessageDirectoryStart() {
    overlayMessage->showMessage("|start", MESSAGE_LEFT, 1000);
}

void MainWindow2::showMessageFitAll() {
    overlayMessage->showMessage("Fit All", MESSAGE_BOTTOM, 1200);
}

void MainWindow2::showMessageFitWidth() {
    overlayMessage->showMessage("Fit Width", MESSAGE_BOTTOM, 1200);
}

void MainWindow2::showMessageFitOriginal() {
    overlayMessage->showMessage("Fit 1:1", MESSAGE_BOTTOM, 1200);
}

void MainWindow2::readSettings() {
    panelPosition = settings->panelPosition();
    sidePanelPosition = settings->sidePanelPosition();
    mainPanelEnabled = settings->mainPanelEnabled();
    sidePanelEnabled = settings->sidePanelEnabled();

    setControlsOverlayEnabled(this->isFullScreen());
    setInfoOverlayEnabled(this->isFullScreen());
}

void MainWindow2::updateOverlayGeometry() {
    controlsOverlay->setContainerSize(size());
    infoOverlay->setContainerSize(size());
    overlayMessage->setContainerSize(size());
    mainPanel->setContainerSize(size());
}

void MainWindow2::setControlsOverlayEnabled(bool mode) {
    if(mode && (panelPosition == BOTTOM || !settings->mainPanelEnabled()))
        controlsOverlay->show();
    else
        controlsOverlay->hide();
}

void MainWindow2::setInfoOverlayEnabled(bool mode) {
    if(mode)
        infoOverlay->show();
    else
        infoOverlay->hide();
}
