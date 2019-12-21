/*
 * This widget combines ImageViewer / VideoPlayer.
 * Only one is displayed at a time.
 */

#include "viewerwidget.h"

ViewerWidget::ViewerWidget(QWidget *parent)
    : FloatingWidgetContainer(parent),
      imageViewer(nullptr),
      videoPlayer(nullptr),
      contextMenu(nullptr),
      mainPanel(nullptr),
      videoControls(nullptr),
      currentWidget(UNSET),
      mInteractionEnabled(false),
      avoidPanelFlag(false),
      mPanelEnabled(false),
      mPanelFullscreenOnly(false),
      mIsFullscreen(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);

    layout.setContentsMargins(0, 0, 0, 0);
    this->setLayout(&layout);

    imageViewer.reset(new ImageViewer(this));
    imageViewer->hide();

    connect(imageViewer.get(), &ImageViewer::scalingRequested, this, &ViewerWidget::scalingRequested);
    connect(this, &ViewerWidget::toggleTransparencyGrid, imageViewer.get(), &ImageViewer::toggleTransparencyGrid);
    connect(this, &ViewerWidget::setFilterNearest,       imageViewer.get(), &ImageViewer::setFilterNearest);
    connect(this, &ViewerWidget::setFilterBilinear,      imageViewer.get(), &ImageViewer::setFilterBilinear);

    videoPlayer.reset(new VideoPlayerInitProxy(this));
    videoPlayer->hide();
    videoControls = new VideoControlsProxyWrapper(this);

    mainPanel.reset(new MainPanel(this));

    connect(videoPlayer.get(), &VideoPlayer::durationChanged, videoControls, &VideoControlsProxyWrapper::setDurationSeconds);
    connect(videoPlayer.get(), &VideoPlayer::positionChanged, videoControls, &VideoControlsProxyWrapper::setPositionSeconds);
    connect(videoPlayer.get(), &VideoPlayer::videoPaused,     videoControls, &VideoControlsProxyWrapper::onVideoPaused);

    connect(videoControls, &VideoControlsProxyWrapper::pause,     this, &ViewerWidget::pauseVideo);
    connect(videoControls, &VideoControlsProxyWrapper::seekLeft,  this, &ViewerWidget::seekVideoLeft);
    connect(videoControls, &VideoControlsProxyWrapper::seekRight, this, &ViewerWidget::seekVideoRight);
    connect(videoControls, &VideoControlsProxyWrapper::seek,      this, &ViewerWidget::seekVideo);
    connect(videoControls, &VideoControlsProxyWrapper::nextFrame, this, &ViewerWidget::frameStep);
    connect(videoControls, &VideoControlsProxyWrapper::prevFrame, this, &ViewerWidget::frameStepBack);

    enableImageViewer();
    enableInteraction();

    connect(&cursorTimer, &QTimer::timeout, this, &ViewerWidget::hideCursor);

    connect(settings, &Settings::settingsChanged, this, &ViewerWidget::readSettings);
    readSettings();
}

QRect ViewerWidget::imageRect() {
    if(imageViewer && currentWidget == IMAGEVIEWER)
        return imageViewer->imageRect();
    else
        return QRect(0,0,0,0);
}

float ViewerWidget::currentScale() {
    if(currentWidget == IMAGEVIEWER)
        return imageViewer->currentScale();
    else
        return 1.0f;
}

QSize ViewerWidget::sourceSize() {
    if(currentWidget == IMAGEVIEWER)
        return imageViewer->sourceSize();
    else
        return QSize(0,0);
}

// hide videoPlayer, show imageViewer
void ViewerWidget::enableImageViewer() {
    if(currentWidget != IMAGEVIEWER) {
        disableVideoPlayer();
        layout.addWidget(imageViewer.get());
        imageViewer->show();
        currentWidget = IMAGEVIEWER;
    }
}

// hide imageViewer, show videoPlayer
void ViewerWidget::enableVideoPlayer() {
    if(currentWidget != VIDEOPLAYER) {
        disableImageViewer();
        layout.addWidget(videoPlayer.get());
        videoPlayer->show();
        currentWidget = VIDEOPLAYER;
    }
}

void ViewerWidget::disableImageViewer() {
    if(currentWidget == IMAGEVIEWER) {
        currentWidget = UNSET;
        imageViewer->closeImage();
        imageViewer->hide();
        layout.removeWidget(imageViewer.get());
    }
}

void ViewerWidget::disableVideoPlayer() {
    if(currentWidget == VIDEOPLAYER) {
        currentWidget = UNSET;
        if(videoControls)
            videoControls->hide();
        videoPlayer->setPaused(true);
        videoPlayer->hide();
        layout.removeWidget(videoPlayer.get());
    }
}

void ViewerWidget::enableInteraction() {
    if(!mInteractionEnabled) {
        connect(this, &ViewerWidget::zoomIn,        imageViewer.get(), &ImageViewer::zoomIn);
        connect(this, &ViewerWidget::zoomOut,       imageViewer.get(), &ImageViewer::zoomOut);
        connect(this, &ViewerWidget::zoomInCursor,  imageViewer.get(), &ImageViewer::zoomInCursor);
        connect(this, &ViewerWidget::zoomOutCursor, imageViewer.get(), &ImageViewer::zoomOutCursor);
        connect(this, &ViewerWidget::scrollUp,      imageViewer.get(), &ImageViewer::scrollUp);
        connect(this, &ViewerWidget::scrollDown,    imageViewer.get(), &ImageViewer::scrollDown);
        connect(this, &ViewerWidget::scrollLeft,    imageViewer.get(), &ImageViewer::scrollLeft);
        connect(this, &ViewerWidget::scrollRight,   imageViewer.get(), &ImageViewer::scrollRight);
        connect(this, &ViewerWidget::fitWindow,     imageViewer.get(), &ImageViewer::setFitWindow);
        connect(this, &ViewerWidget::fitWidth,      imageViewer.get(), &ImageViewer::setFitWidth);
        connect(this, &ViewerWidget::fitOriginal,   imageViewer.get(), &ImageViewer::setFitOriginal);
        connect(imageViewer.get(), &ImageViewer::draggedOut, this, &ViewerWidget::draggedOut);
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        mInteractionEnabled = true;
    }
}

void ViewerWidget::disableInteraction() {
    if(mInteractionEnabled) {
        disconnect(this, &ViewerWidget::zoomIn,        imageViewer.get(), &ImageViewer::zoomIn);
        disconnect(this, &ViewerWidget::zoomOut,       imageViewer.get(), &ImageViewer::zoomOut);
        disconnect(this, &ViewerWidget::zoomInCursor,  imageViewer.get(), &ImageViewer::zoomInCursor);
        disconnect(this, &ViewerWidget::zoomOutCursor, imageViewer.get(), &ImageViewer::zoomOutCursor);
        disconnect(this, &ViewerWidget::scrollUp,      imageViewer.get(), &ImageViewer::scrollUp);
        disconnect(this, &ViewerWidget::scrollDown,    imageViewer.get(), &ImageViewer::scrollDown);
        disconnect(this, &ViewerWidget::scrollLeft,    imageViewer.get(), &ImageViewer::scrollLeft);
        disconnect(this, &ViewerWidget::scrollRight,   imageViewer.get(), &ImageViewer::scrollRight);
        disconnect(this, &ViewerWidget::fitWindow,     imageViewer.get(), &ImageViewer::setFitWindow);
        disconnect(this, &ViewerWidget::fitWidth,      imageViewer.get(), &ImageViewer::setFitWidth);
        disconnect(this, &ViewerWidget::fitOriginal,   imageViewer.get(), &ImageViewer::setFitOriginal);
        disconnect(imageViewer.get(), &ImageViewer::draggedOut, this, &ViewerWidget::draggedOut);
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        hideContextMenu();
        mInteractionEnabled = false;
    }
}

bool ViewerWidget::interactionEnabled() {
    return mInteractionEnabled;
}

std::shared_ptr<DirectoryViewWrapper> ViewerWidget::getPanel() {
    return mainPanel->getWrapper();
}

bool ViewerWidget::showImage(std::unique_ptr<QPixmap> pixmap) {
    if(!pixmap)
        return false;
    stopPlayback();
    enableImageViewer();
    imageViewer->displayImage(std::move(pixmap));
    hideCursorTimed(false);
    return true;
}

bool ViewerWidget::showAnimation(std::unique_ptr<QMovie> movie) {
    if(!movie)
        return false;
    stopPlayback();
    enableImageViewer();
    imageViewer->displayAnimation(std::move(movie));
    hideCursorTimed(false);
    return true;
}

bool ViewerWidget::showVideo(QString file) {
    stopPlayback();
    enableVideoPlayer();
    videoPlayer->openMedia(file);
    hideCursorTimed(false);
    return true;
}

void ViewerWidget::stopPlayback() {
    if(currentWidget == IMAGEVIEWER) {
        imageViewer->stopAnimation();
    }
    if(currentWidget == VIDEOPLAYER) {
        // stopping is visibly slower
        //videoPlayer->stop();
        videoPlayer->setPaused(true);
    }
}

void ViewerWidget::startPlayback() {
    if(currentWidget == IMAGEVIEWER) {
        imageViewer->startAnimation();
    }
    if(currentWidget == VIDEOPLAYER) {
        // stopping is visibly slower
        //videoPlayer->stop();
        videoPlayer->setPaused(false);
    }
}

void ViewerWidget::setFitMode(ImageFitMode mode) {
    if(mode == FIT_WINDOW)
        emit fitWindow();
    else if(mode == FIT_WIDTH)
        emit fitWidth();
    else if(mode == FIT_ORIGINAL)
        emit fitOriginal();
}

ImageFitMode ViewerWidget::fitMode() {
    return imageViewer->fitMode();
}

void ViewerWidget::onScalingFinished(std::unique_ptr<QPixmap> scaled) {
    imageViewer->replacePixmap(std::move(scaled));
}

void ViewerWidget::closeImage() {
    imageViewer->closeImage();
    videoPlayer->stop();
    showCursor();
}

void ViewerWidget::pauseVideo() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->pauseResume();
}

void ViewerWidget::seekVideo(int pos) {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->seek(pos);
}

void ViewerWidget::seekVideoRelative(int pos) {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->seekRelative(pos);
}

void ViewerWidget::seekVideoLeft() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->seekRelative(-10);
}

void ViewerWidget::seekVideoRight() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->seekRelative(10);
}

void ViewerWidget::frameStep() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->frameStep();
}

void ViewerWidget::frameStepBack() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->frameStepBack();
}

void ViewerWidget::toggleMute() {
    if(currentWidget == VIDEOPLAYER) {
        videoPlayer->setMuted(!videoPlayer->muted());
        videoControls->onVideoMuted(videoPlayer->muted());
    }
}

void ViewerWidget::volumeUp() {
    if(currentWidget == VIDEOPLAYER) {
        videoPlayer->volumeUp();
    }
}

void ViewerWidget::volumeDown() {
    if(currentWidget == VIDEOPLAYER) {
        videoPlayer->volumeDown();
    }
}

bool ViewerWidget::isDisplaying() {
    if(currentWidget == IMAGEVIEWER && imageViewer->isDisplaying())
        return true;
    if(currentWidget == VIDEOPLAYER /*&& imageViewer->isDisplaying()*/) // todo
        return true;
    else
        return false;
}

ScalingFilter ViewerWidget::scalingFilter() {
    return imageViewer->scalingFilter();
}

void ViewerWidget::hidePanel() {
    mainPanel->hide();
}

void ViewerWidget::hidePanelAnimated() {
    mainPanel->hideAnimated();
}

PanelHPosition ViewerWidget::panelPosition() {
    return mainPanel->position();
}

bool ViewerWidget::panelEnabled() {
    return mPanelEnabled;
}

void ViewerWidget::mousePressEvent(QMouseEvent *event) {
    hideContextMenu();
    event->ignore();
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent *event) {
    showCursor();
    hideCursorTimed(false);
    event->ignore();
}

void ViewerWidget::mouseMoveEvent(QMouseEvent *event) {
    if(!(event->buttons() & Qt::LeftButton) && !(event->buttons() & Qt::RightButton)) {
        showCursor();
        hideCursorTimed(true);
    }

    // ignore if we are doing something with the mouse (zoom / drag)
    if(event->buttons() != Qt::NoButton) {
        if(mainPanel->triggerRect().contains(event->pos()))
            avoidPanelFlag = true;
        return;
    }

    // show on hover event
    if(mPanelEnabled && (mIsFullscreen|| !mPanelFullscreenOnly)) {
        if(mainPanel->triggerRect().contains(event->pos()) && !avoidPanelFlag) {
            mainPanel->show();
        }
    }

    // fade out on leave event
    if(!mainPanel->isHidden()) {
        // leaveEvent which misfires on HiDPI (rounding error somewhere?)
        // add a few px of buffer area to avoid bugs
        // it still fcks up Fitts law as the buttons are not receiving hover on screen border

        // alright this also only works when in root window. sad.
        if(!mainPanel->triggerRect().adjusted(-8,-8,8,8).contains(event->pos())) {
            mainPanel->hideAnimated();
        }
    }
    if(!mainPanel->triggerRect().contains(event->pos()))
        avoidPanelFlag = false;
    event->ignore();
}

void ViewerWidget::hideCursorTimed(bool restartTimer) {
    if(restartTimer || !cursorTimer.isActive())
        cursorTimer.start(CURSOR_HIDE_TIMEOUT_MS);
}

void ViewerWidget::hideCursor() {
    cursorTimer.stop();
    // only hide when we are under viewer or player widget
    QWidget *w = qApp->widgetAt(QCursor::pos());
    if(w && (w == imageViewer.get() || w == videoPlayer->getPlayer().get()) && isDisplaying()) {
        if(settings->cursorAutohide())
            setCursor(QCursor(Qt::BlankCursor));
        videoControls->hide(); // todo: separate
    }
}

void ViewerWidget::showCursor() {
    cursorTimer.stop();
    if(cursor().shape() == Qt::BlankCursor)
        setCursor(QCursor(Qt::ArrowCursor));
    if(currentWidget == VIDEOPLAYER) {
        videoControls->show();
    }
}

void ViewerWidget::showContextMenu() {
    QPoint pos = cursor().pos();
    showContextMenu(pos);
}

void ViewerWidget::showContextMenu(QPoint pos) {
    if(isVisible() && interactionEnabled()) {
        if(!contextMenu)
            contextMenu.reset(new ContextMenu(this));
        contextMenu->setImageEntriesEnabled(isDisplaying());
        if(!contextMenu->isVisible()) {
            contextMenu->showAt(pos);
        } else {
            contextMenu->hide();
        }
    }
}

void ViewerWidget::onFullscreenModeChanged(bool mode) {
    if(mainPanel->position() == PANEL_TOP)
        mainPanel->setExitButtonEnabled(mode);
    else
        mainPanel->setExitButtonEnabled(false);
    mIsFullscreen = mode;
}

void ViewerWidget::readSettings() {
    mPanelEnabled = settings->panelEnabled();
    mPanelFullscreenOnly = settings->panelFullscreenOnly();
}

void ViewerWidget::hideContextMenu() {
    if(contextMenu)
        contextMenu->hide();
}

void ViewerWidget::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    hideContextMenu();
}

void ViewerWidget::keyPressEvent(QKeyEvent *event) {
    if(currentWidget == VIDEOPLAYER && event->key() == Qt::Key_Space) {
        event->accept();
        videoPlayer->pauseResume();
    } else {
        event->ignore();
    }
}

void ViewerWidget::enterEvent(QEvent *event) {
    QWidget::enterEvent(event);

    if(!mInteractionEnabled)
        return;
    // we can't track move events outside the window (without additional timer),
    // so just hook the panel event here
    if(mPanelEnabled && (mIsFullscreen || !mPanelFullscreenOnly)) {
        if(mainPanel->triggerRect().contains(mapFromGlobal(cursor().pos())) && !avoidPanelFlag) {
            mainPanel->show();
        }
    }
}

void ViewerWidget::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    //qDebug() << cursor().pos() << this->rect();
    // this misfires on hidpi.
    //instead do the panel hiding in MW::leaveEvent  (it works properly in root window)
    //mainPanel->hide();
    avoidPanelFlag = false;
}
