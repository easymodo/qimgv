/*
 * This widget combines ImageViewer / VideoPlayer.
 * Only one is displayed at a time.
 */

#include "viewerwidget.h"

ViewerWidget::ViewerWidget(QWidget *parent)
    : OverlayContainerWidget(parent),
      imageViewer(nullptr),
      videoPlayer(nullptr),
      contextMenu(nullptr),
      currentWidget(UNSET),
      mInteractionEnabled(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);

    layout.setContentsMargins(0, 0, 0, 0);
    this->setLayout(&layout);

    imageViewer.reset(new ImageViewer(this));
    imageViewer->hide();
    connect(imageViewer.get(), SIGNAL(scalingRequested(QSize)),
            this, SIGNAL(scalingRequested(QSize)));
    connect(imageViewer.get(), SIGNAL(rightClicked()),
            this, SLOT(showContextMenu()));

    videoPlayer.reset(new VideoPlayerInitProxy(this));
    videoPlayer->hide();
    videoControls = new VideoControls(this);
    connect(videoPlayer.get(), SIGNAL(durationChanged(int)),
            videoControls, SLOT(setDurationSeconds(int)));
    connect(videoPlayer.get(), SIGNAL(positionChanged(int)),
            videoControls, SLOT(setPositionSeconds(int)));
    connect(videoPlayer.get(), SIGNAL(videoPaused(bool)),
            videoControls, SLOT(onVideoPaused(bool)));
    connect(videoPlayer.get(), SIGNAL(rightClicked()),
            this, SLOT(showContextMenu()));

    connect(videoControls, SIGNAL(pause()), this, SLOT(pauseVideo()));
    connect(videoControls, SIGNAL(seekLeft()), this, SLOT(seekVideoLeft()));
    connect(videoControls, SIGNAL(seekRight()), this, SLOT(seekVideoRight()));
    connect(videoControls, SIGNAL(seek(int)), this, SLOT(seekVideo(int)));
    connect(videoControls, SIGNAL(nextFrame()), this, SLOT(frameStep()));
    connect(videoControls, SIGNAL(prevFrame()), this, SLOT(frameStepBack()));

    enableImageViewer();
    enableInteraction();

    connect(&cursorTimer, SIGNAL(timeout()),
            this, SLOT(hideCursor()), Qt::UniqueConnection);
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
        videoControls->hide();
        videoPlayer->setPaused(true);
        videoPlayer->hide();
        layout.removeWidget(videoPlayer.get());
    }
}

void ViewerWidget::enableInteraction() {
    if(!mInteractionEnabled) {
        connect(this, SIGNAL(zoomIn()), imageViewer.get(), SLOT(zoomIn()));
        connect(this, SIGNAL(zoomOut()), imageViewer.get(), SLOT(zoomOut()));
        connect(this, SIGNAL(zoomInCursor()), imageViewer.get(), SLOT(zoomInCursor()));
        connect(this, SIGNAL(zoomOutCursor()), imageViewer.get(), SLOT(zoomOutCursor()));
        connect(this, SIGNAL(scrollUp()), imageViewer.get(), SLOT(scrollUp()));
        connect(this, SIGNAL(scrollDown()), imageViewer.get(), SLOT(scrollDown()));
        connect(this, SIGNAL(scrollLeft()), imageViewer.get(), SLOT(scrollLeft()));
        connect(this, SIGNAL(scrollRight()), imageViewer.get(), SLOT(scrollRight()));
        connect(this, SIGNAL(fitWindow()), imageViewer.get(), SLOT(setFitWindow()));
        connect(this, SIGNAL(fitWidth()), imageViewer.get(), SLOT(setFitWidth()));
        connect(this, SIGNAL(fitOriginal()), imageViewer.get(), SLOT(setFitOriginal()));
        // block dragging & RMB zoom
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        mInteractionEnabled = true;
    }
}

void ViewerWidget::disableInteraction() {
    if(mInteractionEnabled) {
        disconnect(this, SIGNAL(zoomIn()), imageViewer.get(), SLOT(zoomIn()));
        disconnect(this, SIGNAL(zoomOut()), imageViewer.get(), SLOT(zoomOut()));
        disconnect(this, SIGNAL(zoomInCursor()), imageViewer.get(), SLOT(zoomInCursor()));
        disconnect(this, SIGNAL(zoomOutCursor()), imageViewer.get(), SLOT(zoomOutCursor()));
        disconnect(this, SIGNAL(scrollUp()), imageViewer.get(), SLOT(scrollUp()));
        disconnect(this, SIGNAL(scrollDown()), imageViewer.get(), SLOT(scrollDown()));
        disconnect(this, SIGNAL(fitWindow()), imageViewer.get(), SLOT(setFitWindow()));
        disconnect(this, SIGNAL(fitWidth()), imageViewer.get(), SLOT(setFitWidth()));
        disconnect(this, SIGNAL(fitOriginal()), imageViewer.get(), SLOT(setFitOriginal()));
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        hideContextMenu();
        mInteractionEnabled = false;
    }
}

bool ViewerWidget::interactionEnabled() {
    return mInteractionEnabled;
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

bool ViewerWidget::showVideo(Clip *clip) {
    if(!clip)
        return false;
    stopPlayback();
    enableVideoPlayer();
    videoPlayer->openMedia(clip);
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

bool ViewerWidget::isDisplaying() {
    if(currentWidget == IMAGEVIEWER && imageViewer->isDisplaying())
        return true;
    if(currentWidget == VIDEOPLAYER /*&& imageViewer->isDisplaying()*/) // todo
        return true;
    else
        return false;
}

void ViewerWidget::mousePressEvent(QMouseEvent *event) {
    event->ignore();
    hideContextMenu();
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
    event->ignore();
}

void ViewerWidget::hideCursorTimed(bool restartTimer) {
    if(restartTimer || !cursorTimer.isActive())
        cursorTimer.start(CURSOR_HIDE_TIMEOUT_MS);
}

void ViewerWidget::hideCursor() {
    cursorTimer.stop();
    // checking overlays explicitly is a bit ugly
    // todo: find a better solution without reparenting
    // maybe keep a list of pointers in OverlayContainerWidget on overlay attach?
    if(this->underMouse() && !videoControls->underMouse() && isDisplaying()) {
        setCursor(QCursor(Qt::BlankCursor));
        videoControls->hide();
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
    showContextMenu(cursor().pos());
}

void ViewerWidget::showContextMenu(QPoint pos) {
    if(interactionEnabled()) {
        if(!contextMenu)
            contextMenu.reset(new ContextMenu());
        contextMenu->setImageEntriesEnabled(isDisplaying());
        contextMenu->showAt(pos);
    }
}

void ViewerWidget::hideContextMenu() {
    if(!contextMenu)
        contextMenu.reset(new ContextMenu());
    contextMenu->hide();
}

void ViewerWidget::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    hideContextMenu();
}
