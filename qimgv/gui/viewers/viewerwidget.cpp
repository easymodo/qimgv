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
      videoControls(nullptr),
      currentWidget(UNSET),
      mInteractionEnabled(false),
      mWaylandCursorWorkaround(false),
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
    connect(this, &ViewerWidget::setScalingFilter,       imageViewer.get(), &ImageViewerV2::setScalingFilter);


    videoPlayer.reset(new VideoPlayerInitProxy(this));
    layout.addWidget(videoPlayer.get());
    videoPlayer->hide();
    videoControls = new VideoControlsProxyWrapper(this);

    // tmp no wrapper
    zoomIndicator = new ZoomIndicatorOverlayProxy(this);
    clickZoneOverlay = new ClickZoneOverlay(this);

    connect(videoPlayer.get(), &VideoPlayer::playbackFinished, this, &ViewerWidget::onVideoPlaybackFinished);

    connect(videoControls, &VideoControlsProxyWrapper::seekBackward,  this, &ViewerWidget::seekBackward);
    connect(videoControls, &VideoControlsProxyWrapper::seekForward, this, &ViewerWidget::seekForward);
    connect(videoControls, &VideoControlsProxyWrapper::seek,      this, &ViewerWidget::seek);

    enableImageViewer();
    setInteractionEnabled(true);

    connect(&cursorTimer, &QTimer::timeout, this, &ViewerWidget::hideCursor);

    connect(settings, &Settings::settingsChanged, this, &ViewerWidget::readSettings);
    readSettings();
}

QRect ViewerWidget::imageRect() {
    if(imageViewer && currentWidget == IMAGEVIEWER)
        return imageViewer->scaledRectR();
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

void ViewerWidget::setInteractionEnabled(bool mode) {
    if(mInteractionEnabled == mode)
        return;
    mInteractionEnabled = mode;
    if(mInteractionEnabled) {
        connect(this, &ViewerWidget::toggleLockZoom, imageViewer.get(), &ImageViewerV2::toggleLockZoom);
        connect(this, &ViewerWidget::toggleLockView, imageViewer.get(), &ImageViewerV2::toggleLockView);
        connect(this, &ViewerWidget::zoomIn,         imageViewer.get(), &ImageViewerV2::zoomIn);
        connect(this, &ViewerWidget::zoomOut,        imageViewer.get(), &ImageViewerV2::zoomOut);
        connect(this, &ViewerWidget::zoomInCursor,   imageViewer.get(), &ImageViewerV2::zoomInCursor);
        connect(this, &ViewerWidget::zoomOutCursor,  imageViewer.get(), &ImageViewerV2::zoomOutCursor);
        connect(this, &ViewerWidget::scrollUp,       imageViewer.get(), &ImageViewerV2::scrollUp);
        connect(this, &ViewerWidget::scrollDown,     imageViewer.get(), &ImageViewerV2::scrollDown);
        connect(this, &ViewerWidget::scrollLeft,     imageViewer.get(), &ImageViewerV2::scrollLeft);
        connect(this, &ViewerWidget::scrollRight,    imageViewer.get(), &ImageViewerV2::scrollRight);
        connect(this, &ViewerWidget::fitWindow,      imageViewer.get(), &ImageViewerV2::setFitWindow);
        connect(this, &ViewerWidget::fitWidth,       imageViewer.get(), &ImageViewerV2::setFitWidth);
        connect(this, &ViewerWidget::fitOriginal,    imageViewer.get(), &ImageViewerV2::setFitOriginal);
        connect(imageViewer.get(), &ImageViewerV2::draggedOut, this, &ViewerWidget::draggedOut);
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    } else {
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
    }
}

bool ViewerWidget::interactionEnabled() {
    return mInteractionEnabled;
}

bool ViewerWidget::showImage(std::unique_ptr<QPixmap> pixmap) {
    if(!pixmap)
        return false;
    stopPlayback();
    videoControls->hide();
    enableImageViewer();
    imageViewer->showImage(std::move(pixmap));
    hideCursorTimed(false);
    return true;
}

bool ViewerWidget::showAnimation(std::shared_ptr<QMovie> movie) {
    if(!movie)
        return false;
    stopPlayback();
    enableImageViewer();
    imageViewer->showAnimation(movie);
    hideCursorTimed(false);
    return true;
}

bool ViewerWidget::showVideo(QString file) {
    stopPlayback();
    enableVideoPlayer();
    videoPlayer->showVideo(file);
    hideCursorTimed(false);
    return true;
}

void ViewerWidget::stopPlayback() {
    if(currentWidget == IMAGEVIEWER && imageViewer->hasAnimation())
        imageViewer->stopAnimation();
    if(currentWidget == VIDEOPLAYER) {
        // stopping is visibly slower
        //videoPlayer->stop();
        videoPlayer->setPaused(true);
    }
}

void ViewerWidget::startPlayback() {
    if(currentWidget == IMAGEVIEWER && imageViewer->hasAnimation())
        imageViewer->startAnimation();
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

void ViewerWidget::seekBackward() {
    if(currentWidget == VIDEOPLAYER)
        videoPlayer.get()->seekRelative(-10);
}

void ViewerWidget::seekForward() {
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
    if(currentWidget == VIDEOPLAYER)
        videoPlayer->volumeUp();
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

bool ViewerWidget::lockZoomEnabled() {
    return imageViewer->lockZoomEnabled();
}

bool ViewerWidget::lockViewEnabled() {
    return imageViewer->lockViewEnabled();
}

ScalingFilter ViewerWidget::scalingFilter() {
    return imageViewer->scalingFilter();
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
            QPoint posMapped = mapFromGlobal(QCursor::pos());
            //if(settings->enableClickZoneThing())
            // ignore when we are hovering the click zone
            if(clickZoneOverlay->leftZone().contains(posMapped) ||
                clickZoneOverlay->leftZone().contains(posMapped))
            {
                return;
            }

            // only hide when we are under viewer or player widget
            QWidget *w = qApp->widgetAt(QCursor::pos());
            if(w && (w == imageViewer.get()->viewport() || w == videoPlayer->getPlayer().get())) {
                if(!videoControls->isVisible() || !videoControlsArea().contains(posMapped)) {
                    setCursor(QCursor(Qt::BlankCursor));
                    videoControls->hide();
                }
            }
        }
    }
}

QRect ViewerWidget::videoControlsArea() {
    QRect vcontrolsRect;
    if(settings->panelEnabled() && settings->panelPosition() == PANEL_BOTTOM)
        vcontrolsRect = QRect(0, 0, width(), 160); // inverted (top)
    else
        vcontrolsRect = QRect(0, height() - 160, width(), height());
    return vcontrolsRect;
}

// click zone input crutch
// --
// we can't process mouse events in the overlay
// cause they won't propagate to the ImageViewer, only to overlay's container (this widget)
// so we just grab them before they reach ImageViewer and do the needful
bool ViewerWidget::eventFilter(QObject *object, QEvent *event) {
    // catch press and doubleclick
    // force doubleclick to act as press event for click zones
    if(event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
        // disable feature for very small windows
        if(width() <= 250)
            return false;

        auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if(mouseEvent->button() != Qt::LeftButton || mouseEvent->modifiers()) {
            clickZoneOverlay->disableHighlight();
            return false;
        }
        if(clickZoneOverlay->leftZone().contains(mouseEvent->pos())) {
            clickZoneOverlay->setPressed(true);
            clickZoneOverlay->highlightLeft();
            imageViewer.get()->disableDrags();
            actionManager->invokeAction("prevImage");
            return true; // do not pass the event to imageViewer
        }
        if(clickZoneOverlay->rightZone().contains(mouseEvent->pos())) {
            clickZoneOverlay->setPressed(true);
            clickZoneOverlay->highlightRight();
            imageViewer.get()->disableDrags();
            actionManager->invokeAction("nextImage");
            return true;
        }
    }
    // right click produces QEvent::ContextMenu instead of QEvent::MouseButtonPress
    // this is NOT a QMouseEvent
    if(event->type() == QEvent::ContextMenu) {
        clickZoneOverlay->disableHighlight();
        return false;
    }

    if(event->type() == QEvent::MouseButtonRelease) {
        clickZoneOverlay->setPressed(false);
        imageViewer.get()->enableDrags();
    }

    if(event->type() == QEvent::MouseMove || event->type() == QEvent::Enter) {
        QPoint mousePos;
        if(event->type() == QEvent::MouseMove) {
            auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
            mousePos = mouseEvent->pos();
            if(mouseEvent->buttons())
                return false;
        } else {
            auto enterEvent = dynamic_cast<QEnterEvent*>(event);
            mousePos = enterEvent->pos();
        }
        if(clickZoneOverlay->leftZone().contains(mousePos)) {
            clickZoneOverlay->setPressed(false);
            clickZoneOverlay->highlightLeft();
            setCursor(Qt::PointingHandCursor);
            return true;
        } else if(clickZoneOverlay->rightZone().contains(mousePos)) {
            clickZoneOverlay->setPressed(false);
            clickZoneOverlay->highlightRight();
            setCursor(Qt::PointingHandCursor);
            return true;
        } else {
            clickZoneOverlay->disableHighlight();
            setCursor(Qt::ArrowCursor);
        }
    }

    if(event->type() == QEvent::Leave) {
        clickZoneOverlay->disableHighlight();
        setCursor(Qt::ArrowCursor);
    }

    return false; // send event to imageViewer / videoplayer
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
        if(!contextMenu) {
            contextMenu.reset(new ContextMenu(this));
            connect(contextMenu.get(), &ContextMenu::showScriptSettings, this, &ViewerWidget::showScriptSettings);
        }
        contextMenu->setImageEntriesEnabled(isDisplaying());
        if(!contextMenu->isVisible())
            contextMenu->showAt(pos);
        else
            contextMenu->hide();
    }
}

void ViewerWidget::onFullscreenModeChanged(bool mode) {
    imageViewer->onFullscreenModeChanged(mode);
    mIsFullscreen = mode;
}

void ViewerWidget::readSettings() {
    videoControls->onVideoMuted(!settings->playVideoSounds());
    if(settings->clickableEdges()) {
        imageViewer->viewport()->installEventFilter(this);
        videoPlayer->installEventFilter(this);
        clickZoneOverlay->show();
    } else {
        imageViewer->viewport()->removeEventFilter(this);
        videoPlayer->removeEventFilter(this);
        imageViewer.get()->enableDrags();
        clickZoneOverlay->hide();
    }
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
    if(currentWidget == VIDEOPLAYER && event->key() == Qt::Key_Space) {
        videoPlayer->pauseResume();
        return;
    }
    if(currentWidget == IMAGEVIEWER && imageViewer->isDisplaying()) {
        // switch to fitWidth via up arrow
        if(ShortcutBuilder::fromEvent(event) == "Up" && !actionManager->actionForShortcut("Up").isEmpty()) {
            if(imageViewer->fitMode() == FIT_WINDOW && imageViewer->scaledImageFits()) {
                imageViewer->setFitWidth();
                return;
            }
        }
    }
    actionManager->processEvent(event);
}

void ViewerWidget::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    videoControls->hide();
}
