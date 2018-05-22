/*
 * This widget combines ImageViewer / VideoPlayer.
 * Only one is displayed at a time.
 */

#include "viewerwidget.h"

ViewerWidget::ViewerWidget(QWidget *parent)
    : ContainerWidget(parent),
      imageViewer(nullptr),
      videoPlayer(nullptr),
      currentWidget(UNSET),
      bgOpacity(1.0f),
      zoomInteraction(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setMouseTracking(true);
    layout.setContentsMargins(0, 0, 0, 0);
    this->setLayout(&layout);

    imageViewer.reset(new ImageViewer(this));
    imageViewer->hide();
    connect(imageViewer.get(), SIGNAL(scalingRequested(QSize)), this, SIGNAL(scalingRequested(QSize)));

    videoPlayer.reset(new VideoPlayerInitProxy(this));

    videoPlayer->hide();

    enableImageViewer();
    enableZoomInteraction();
    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
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
        if(currentWidget == VIDEOPLAYER) {
            videoPlayer->setPaused(true);
            videoPlayer->hide();
            layout.removeWidget(videoPlayer.get());
        }
        layout.addWidget(imageViewer.get());
        imageViewer->show();
        currentWidget = IMAGEVIEWER;
    }
}

// hide imageViewer, show videoPlayer
void ViewerWidget::enableVideoPlayer() {
    if(currentWidget != VIDEOPLAYER) {
        if(currentWidget == IMAGEVIEWER) {
            imageViewer->closeImage();
            imageViewer->hide();
            layout.removeWidget(imageViewer.get());
        }
        layout.addWidget(videoPlayer.get());
        videoPlayer->show();
        currentWidget = VIDEOPLAYER;
    }
}

void ViewerWidget::readSettings() {
    bgColor = settings->backgroundColor();
    bgOpacity = settings->backgroundOpacity();
    update();
}

void ViewerWidget::enableZoomInteraction() {
    if(!zoomInteraction) {
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
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        zoomInteraction = true;
    }
}

void ViewerWidget::disableZoomInteraction() {
    if(zoomInteraction) {
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
        zoomInteraction = false;
    }
}

bool ViewerWidget::zoomInteractionEnabled() {
    return zoomInteraction;
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

bool ViewerWidget::showFolderView() {

}

void ViewerWidget::stopPlayback() {
    if(currentWidget == IMAGEVIEWER)
        imageViewer->stopAnimation();
    if(currentWidget == VIDEOPLAYER)
        videoPlayer->setPaused(true);
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
    showCursor();
    // TODO: implement this
    //videoPlayer->closeVideo();
}

void ViewerWidget::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setOpacity(bgOpacity);
    p.setBrush(QBrush(bgColor));
    p.fillRect(this->rect(), p.brush());
}

void ViewerWidget::mousePressEvent(QMouseEvent *event) {
    showCursor();
    // supports zoom/pan
    if(currentWidget == IMAGEVIEWER) {
        if(event->button() == Qt::LeftButton) {
            setCursor(QCursor(Qt::ClosedHandCursor));
        }
        if(event->button() == Qt::RightButton) {
            setCursor(QCursor(Qt::SizeVerCursor));
        }
    }
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent *event) {
    showCursor();
    hideCursorTimed(false);
}

void ViewerWidget::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
    } else if(event->buttons() & Qt::RightButton) {
    } else {
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
    if(this->underMouse())
        setCursor(QCursor(Qt::BlankCursor));
}

void ViewerWidget::showCursor() {
    cursorTimer.stop();
    setCursor(QCursor(Qt::ArrowCursor));
}
