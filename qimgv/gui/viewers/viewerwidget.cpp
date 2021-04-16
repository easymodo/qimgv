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
      mWaylandCursorWorkaround(false),
      avoidPanelFlag(false),
      mPanelEnabled(false),
      mPanelFullscreenOnly(false),
      mIsFullscreen(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
#ifdef Q_OS_LINUX
    // we cant check cursor position on wayland until the mouse is moved
    // use this to skip cursor check once
    if(qgetenv("XDG_SESSION_TYPE") == "wayland")
        mWaylandCursorWorkaround = true;
#endif
    layout.setContentsMargins(0, 0, 0, 0);
    layout.setSpacing(0);
    this->setLayout(&layout);

    imageViewer.reset(new ImageViewerV2(this));
    layout.addWidget(imageViewer.get());
    imageViewer->hide();

    connect(imageViewer.get(), &ImageViewerV2::scalingRequested, this, &ViewerWidget::scalingRequested);
    connect(imageViewer.get(), &ImageViewerV2::scaleChanged, this, &ViewerWidget::onScaleChanged);
    connect(imageViewer.get(), &ImageViewerV2::playbackFinished, this, &ViewerWidget::onAnimationPlaybackFinished);
    connect(this, &ViewerWidget::toggleTransparencyGrid, imageViewer.get(), &ImageViewerV2::toggleTransparencyGrid);
    connect(this, &ViewerWidget::setFilterNearest,       imageViewer.get(), &ImageViewerV2::setFilterNearest);
    connect(this, &ViewerWidget::setFilterBilinear,      imageViewer.get(), &ImageViewerV2::setFilterBilinear);


    videoPlayer.reset(new VideoPlayerInitProxy(this));
    layout.addWidget(videoPlayer.get());
    videoPlayer->hide();
    videoControls = new VideoControlsProxyWrapper(this);

    // tmp no wrapper
    zoomIndicator = new ZoomIndicatorOverlayProxy(this);

    mainPanel.reset(new MainPanel(this));

    connect(videoPlayer.get(), &VideoPlayer::playbackFinished, this, &ViewerWidget::onVideoPlaybackFinished);

    connect(videoControls, &VideoControlsProxyWrapper::seekLeft,  this, &ViewerWidget::seekLeft);
    connect(videoControls, &VideoControlsProxyWrapper::seekRight, this, &ViewerWidget::seekRight);
    connect(videoControls, &VideoControlsProxyWrapper::seek,      this, &ViewerWidget::seek);

    enableImageViewer();
    enableInteraction();

    connect(&cursorTimer, &QTimer::timeout, this, &ViewerWidget::hideCursor);

    connect(settings, &Settings::settingsChanged, this, &ViewerWidget::readSettings);
    readSettings();
}

QRect ViewerWidget::imageRect() {
    if(imageViewer && currentWidget == IMAGEVIEWER)
        return imageViewer->scaledRect();
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
        videoControls->setMode(PLAYBACK_ANIMATION);
        connect(imageViewer.get(), &ImageViewerV2::durationChanged, videoControls, &VideoControlsProxyWrapper::setPlaybackDuration);
        connect(imageViewer.get(), &ImageViewerV2::frameChanged,    videoControls, &VideoControlsProxyWrapper::setPlaybackPosition);
        connect(imageViewer.get(), &ImageViewerV2::animationPaused, videoControls, &VideoControlsProxyWrapper::onPlaybackPaused);
        imageViewer->show();
        currentWidget = IMAGEVIEWER;
    }
}

// hide imageViewer, show videoPlayer
void ViewerWidget::enableVideoPlayer() {
    if(currentWidget != VIDEOPLAYER) {
        disableImageViewer();
        videoControls->setMode(PLAYBACK_VIDEO);
        connect(videoPlayer.get(), &VideoPlayer::durationChanged, videoControls, &VideoControlsProxyWrapper::setPlaybackDuration);
        connect(videoPlayer.get(), &VideoPlayer::positionChanged, videoControls, &VideoControlsProxyWrapper::setPlaybackPosition);
        connect(videoPlayer.get(), &VideoPlayer::videoPaused,     videoControls, &VideoControlsProxyWrapper::onPlaybackPaused);
        videoPlayer->show();
        currentWidget = VIDEOPLAYER;
    }
}

void ViewerWidget::disableImageViewer() {
    if(currentWidget == IMAGEVIEWER) {
        currentWidget = UNSET;
        imageViewer->closeImage();
        imageViewer->hide();
        zoomIndicator->hide();
        disconnect(imageViewer.get(), &ImageViewerV2::durationChanged, videoControls, &VideoControlsProxyWrapper::setPlaybackDuration);
        disconnect(imageViewer.get(), &ImageViewerV2::frameChanged,    videoControls, &VideoControlsProxyWrapper::setPlaybackPosition);
        disconnect(imageViewer.get(), &ImageViewerV2::animationPaused, videoControls, &VideoControlsProxyWrapper::onPlaybackPaused);
    }
}

void ViewerWidget::disableVideoPlayer() {
    if(currentWidget == VIDEOPLAYER) {
        currentWidget = UNSET;
        //videoControls->hide();
        disconnect(videoPlayer.get(), &VideoPlayer::durationChanged, videoControls, &VideoControlsProxyWrapper::setPlaybackDuration);
        disconnect(videoPlayer.get(), &VideoPlayer::positionChanged, videoControls, &VideoControlsProxyWrapper::setPlaybackPosition);
        disconnect(videoPlayer.get(), &VideoPlayer::videoPaused,     videoControls, &VideoControlsProxyWrapper::onPlaybackPaused);
        videoPlayer->setPaused(true);
        // even after calling hide() the player sends a few video frames
        // which paints over the imageviewer, causing corruption
        // so we do not HIDE it, but rather just cover it by imageviewer's widget
        // seems to work fine, might even feel a bit snappier
        if(!videoPlayer->isInitialized())
            videoPlayer->hide();
    }
}

void ViewerWidget::onScaleChanged(qreal scale) {
    if(!this->isVisible())
        return;
    if(scale != 1.0f) {
        zoomIndicator->setScale(scale);
        if(settings->zoomIndicatorMode() == ZoomIndicatorMode::INDICATOR_ENABLED)
            zoomIndicator->show();
        else if((settings->zoomIndicatorMode() == ZoomIndicatorMode::INDICATOR_AUTO))
            zoomIndicator->show(1500);
    } else {
        zoomIndicator->hide();
    }
}

void ViewerWidget::onVideoPlaybackFinished() {
    if(currentWidget == VIDEOPLAYER)
        emit playbackFinished();
}

void ViewerWidget::onAnimationPlaybackFinished() {
    if(currentWidget == IMAGEVIEWER)
        emit playbackFinished();
}

void ViewerWidget::enableInteraction() {
    if(!mInteractionEnabled) {
        connect(this, &ViewerWidget::zoomIn,        imageViewer.get(), &ImageViewerV2::zoomIn);
        connect(this, &ViewerWidget::zoomOut,       imageViewer.get(), &ImageViewerV2::zoomOut);
        connect(this, &ViewerWidget::zoomInCursor,  imageViewer.get(), &ImageViewerV2::zoomInCursor);
        connect(this, &ViewerWidget::zoomOutCursor, imageViewer.get(), &ImageViewerV2::zoomOutCursor);
        connect(this, &ViewerWidget::scrollUp,      imageViewer.get(), &ImageViewerV2::scrollUp);
        connect(this, &ViewerWidget::scrollDown,    imageViewer.get(), &ImageViewerV2::scrollDown);
        connect(this, &ViewerWidget::scrollLeft,    imageViewer.get(), &ImageViewerV2::scrollLeft);
        connect(this, &ViewerWidget::scrollRight,   imageViewer.get(), &ImageViewerV2::scrollRight);
        connect(this, &ViewerWidget::fitWindow,     imageViewer.get(), &ImageViewerV2::setFitWindow);
        connect(this, &ViewerWidget::fitWidth,      imageViewer.get(), &ImageViewerV2::setFitWidth);
        connect(this, &ViewerWidget::fitOriginal,   imageViewer.get(), &ImageViewerV2::setFitOriginal);
        connect(imageViewer.get(), &ImageViewerV2::draggedOut, this, &ViewerWidget::draggedOut);
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        mInteractionEnabled = true;
    }
}

void ViewerWidget::disableInteraction() {
    if(mInteractionEnabled) {
        disconnect(this, &ViewerWidget::zoomIn,        imageViewer.get(), &ImageViewerV2::zoomIn);
        disconnect(this, &ViewerWidget::zoomOut,       imageViewer.get(), &ImageViewerV2::zoomOut);
        disconnect(this, &ViewerWidget::zoomInCursor,  imageViewer.get(), &ImageViewerV2::zoomInCursor);
        disconnect(this, &ViewerWidget::zoomOutCursor, imageViewer.get(), &ImageViewerV2::zoomOutCursor);
        disconnect(this, &ViewerWidget::scrollUp,      imageViewer.get(), &ImageViewerV2::scrollUp);
        disconnect(this, &ViewerWidget::scrollDown,    imageViewer.get(), &ImageViewerV2::scrollDown);
        disconnect(this, &ViewerWidget::scrollLeft,    imageViewer.get(), &ImageViewerV2::scrollLeft);
        disconnect(this, &ViewerWidget::scrollRight,   imageViewer.get(), &ImageViewerV2::scrollRight);
        disconnect(this, &ViewerWidget::fitWindow,     imageViewer.get(), &ImageViewerV2::setFitWindow);
        disconnect(this, &ViewerWidget::fitWidth,      imageViewer.get(), &ImageViewerV2::setFitWidth);
        disconnect(this, &ViewerWidget::fitOriginal,   imageViewer.get(), &ImageViewerV2::setFitOriginal);
        disconnect(imageViewer.get(), &ImageViewerV2::draggedOut, this, &ViewerWidget::draggedOut);
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        hideContextMenu();
        mInteractionEnabled = false;
    }
}

bool ViewerWidget::interactionEnabled() {
    return mInteractionEnabled;
}

std::shared_ptr<ThumbnailStripProxy> ViewerWidget::getThumbPanel() {
    return mainPanel->getThumbnailStrip();
}

bool ViewerWidget::showImage(std::unique_ptr<QPixmap> pixmap) {
    if(!pixmap)
        return false;
    stopPlayback();
    videoControls->hide();
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
    if(currentWidget == IMAGEVIEWER && imageViewer->hasAnimation()) {
        imageViewer->stopAnimation();
    }
    if(currentWidget == VIDEOPLAYER) {
        // stopping is visibly slower
        //videoPlayer->stop();
        videoPlayer->setPaused(true);
    }
}

void ViewerWidget::startPlayback() {
    if(currentWidget == IMAGEVIEWER && imageViewer->hasAnimation()) {
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
    imageViewer->setScaledPixmap(std::move(scaled));
}

void ViewerWidget::closeImage() {
    imageViewer->closeImage();
    videoPlayer->stop();
    showCursor();
}

void ViewerWidget::pauseResumePlayback() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->pauseResume();
    else if(imageViewer->hasAnimation())
        imageViewer->pauseResume();
}

void ViewerWidget::seek(int pos) {
    if(currentWidget == VIDEOPLAYER) {
        videoPlayer.get()->seek(pos);
    } else if(imageViewer->hasAnimation()) {
        imageViewer->stopAnimation();
        imageViewer->showAnimationFrame(pos);
    }
}

void ViewerWidget::seekRelative(int pos) {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->seekRelative(pos);
}

void ViewerWidget::seekLeft() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->seekRelative(-10);
}

void ViewerWidget::seekRight() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->seekRelative(10);
}

void ViewerWidget::frameStep() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->frameStep();
    else if(imageViewer->hasAnimation()) {
        imageViewer->stopAnimation();
        imageViewer->nextFrame();
    }
}

void ViewerWidget::frameStepBack() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->frameStepBack();
    else if(imageViewer->hasAnimation()) {
        imageViewer->stopAnimation();
        imageViewer->prevFrame();
    }
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
    if(currentWidget == VIDEOPLAYER)
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

void ViewerWidget::setupMainPanel() {
    if(mPanelEnabled)
        mainPanel->setupThumbnailStrip();
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
    mWaylandCursorWorkaround = false;
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

    if(currentWidget == VIDEOPLAYER || imageViewer->hasAnimation()) {
        if(videoControlsArea().contains(event->pos()))
            videoControls->show();
        else
            videoControls->hide();
    }

    event->ignore();
}

void ViewerWidget::hideCursorTimed(bool restartTimer) {
    if(restartTimer || !cursorTimer.isActive())
        cursorTimer.start(CURSOR_HIDE_TIMEOUT_MS);
}

void ViewerWidget::hideCursor() {
    cursorTimer.stop();
    // ignore if we have something else open like settings window
    if(!isDisplaying() || !isActiveWindow())
        return;
    // ignore when menu is up
    if(contextMenu && contextMenu->isVisible())
        return;
    if(settings->cursorAutohide()) {
        // force hide on wayland until we can get the cursor pos
        if(mWaylandCursorWorkaround) {
            setCursor(QCursor(Qt::BlankCursor));
            videoControls->hide();
        } else {
            // only hide when we are under viewer or player widget
            QWidget *w = qApp->widgetAt(QCursor::pos());
            if(w && (w == imageViewer.get()->viewport() || w == videoPlayer->getPlayer().get())) {
                if(!videoControls->isVisible() || !videoControlsArea().contains(mapFromGlobal(QCursor::pos()))) {
                    setCursor(QCursor(Qt::BlankCursor));
                    videoControls->hide();
                }
            }
        }
    }
}

QRect ViewerWidget::videoControlsArea() {
    QRect vcontrolsRect;
    if(!mPanelEnabled || mainPanel->position() == PANEL_TOP)
        vcontrolsRect = QRect(0, height() - 160, width(), height());
    else
        vcontrolsRect = QRect(0, 0, width(), 160);
    return vcontrolsRect;
}

void ViewerWidget::showCursor() {
    cursorTimer.stop();
    if(cursor().shape() == Qt::BlankCursor)
        setCursor(QCursor(Qt::ArrowCursor));
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
    imageViewer->onFullscreenModeChanged(mode);
    if(mainPanel->position() == PANEL_TOP)
        mainPanel->setExitButtonEnabled(mode);
    else
        mainPanel->setExitButtonEnabled(false);
    mIsFullscreen = mode;
}

void ViewerWidget::readSettings() {
    mPanelEnabled = settings->panelEnabled();
    mPanelFullscreenOnly = settings->panelFullscreenOnly();
    videoControls->onVideoMuted(!settings->playVideoSounds());
}

void ViewerWidget::setLoopPlayback(bool mode) {
    imageViewer->setLoopPlayback(mode);
    videoPlayer->setLoopPlayback(mode);
}

void ViewerWidget::hideContextMenu() {
    if(contextMenu)
        contextMenu->hide();
}

void ViewerWidget::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    hideContextMenu();
}

// block native tab-switching so we can use it in shortcuts
bool ViewerWidget::focusNextPrevChild(bool mode) {
    return false;
}

void ViewerWidget::keyPressEvent(QKeyEvent *event) {
    event->accept();
    if(currentWidget == VIDEOPLAYER && event->key() == Qt::Key_Space)
        videoPlayer->pauseResume();
    else
        actionManager->processEvent(event);
}

void ViewerWidget::enterEvent(QEnterEvent *event) {
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

    videoControls->hide();
}
