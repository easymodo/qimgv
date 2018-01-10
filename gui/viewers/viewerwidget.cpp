/*
 * This widget combines ImageViewer / VideoPlayer.
 * Only one is displayed at a time.
 */

#include "viewerwidget.h"

ViewerWidget::ViewerWidget(QWidget *parent)
    : ContainerWidget(parent),
      imageViewer(NULL),
      videoPlayer(NULL),
      currentWidget(UNSET),
      zoomInteraction(false)
{
    this->setMouseTracking(true);
    layout.setContentsMargins(0, 0, 0, 0);
    this->setLayout(&layout);

    imageViewer = new ImageViewer(this);
    imageViewer->hide();
    connect(imageViewer, SIGNAL(scalingRequested(QSize)), this, SIGNAL(scalingRequested(QSize)));

    videoPlayer = new VideoPlayerMpvProxy(this);
    videoPlayer->hide();

    enableImageViewer();
    enableZoomInteraction();
    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

QRectF ViewerWidget::imageRect() {
    if(imageViewer && currentWidget == IMAGEVIEWER)
        return imageViewer->imageRect();
    else
        return QRectF(0,0,0,0);
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
            layout.removeWidget(videoPlayer);
        }
        layout.addWidget(imageViewer);
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
            layout.removeWidget(imageViewer);
        }
        layout.addWidget(videoPlayer);
        videoPlayer->show();
        currentWidget = VIDEOPLAYER;
    }
}

void ViewerWidget::readSettings() {
    bgColor = settings->backgroundColor();
    update();
}

void ViewerWidget::enableZoomInteraction() {
    if(!zoomInteraction) {
        connect(this, SIGNAL(zoomIn()), imageViewer, SLOT(zoomIn()));
        connect(this, SIGNAL(zoomOut()), imageViewer, SLOT(zoomOut()));
        connect(this, SIGNAL(zoomInCursor()), imageViewer, SLOT(zoomInCursor()));
        connect(this, SIGNAL(zoomOutCursor()), imageViewer, SLOT(zoomOutCursor()));
        connect(this, SIGNAL(scrollUp()), imageViewer, SLOT(scrollUp()));
        connect(this, SIGNAL(scrollDown()), imageViewer, SLOT(scrollDown()));
        connect(this, SIGNAL(fitWindow()), imageViewer, SLOT(setFitWindow()));
        connect(this, SIGNAL(fitWidth()), imageViewer, SLOT(setFitWidth()));
        connect(this, SIGNAL(fitOriginal()), imageViewer, SLOT(setFitOriginal()));
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        zoomInteraction = true;
    }
}

void ViewerWidget::disableZoomInteraction() {
    if(zoomInteraction) {
        disconnect(this, SIGNAL(zoomIn()), imageViewer, SLOT(zoomIn()));
        disconnect(this, SIGNAL(zoomOut()), imageViewer, SLOT(zoomOut()));
        disconnect(this, SIGNAL(zoomInCursor()), imageViewer, SLOT(zoomInCursor()));
        disconnect(this, SIGNAL(zoomOutCursor()), imageViewer, SLOT(zoomOutCursor()));
        disconnect(this, SIGNAL(scrollUp()), imageViewer, SLOT(scrollUp()));
        disconnect(this, SIGNAL(scrollDown()), imageViewer, SLOT(scrollDown()));
        disconnect(this, SIGNAL(fitWindow()), imageViewer, SLOT(setFitWindow()));
        disconnect(this, SIGNAL(fitWidth()), imageViewer, SLOT(setFitWidth()));
        disconnect(this, SIGNAL(fitOriginal()), imageViewer, SLOT(setFitOriginal()));
        imageViewer->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        zoomInteraction = false;
    }
}

bool ViewerWidget::zoomInteractionEnabled() {
    return zoomInteraction;
}

bool ViewerWidget::showImage(QPixmap *pixmap) {
    if(!pixmap)
        return false;
    stopPlayback();
    enableImageViewer();
    imageViewer->displayImage(pixmap);
    return true;
}

bool ViewerWidget::showAnimation(QMovie *movie) {
    if(!movie)
        return false;
    stopPlayback();
    enableImageViewer();
    imageViewer->displayAnimation(movie);
    return true;
}

bool ViewerWidget::showVideo(Clip *clip) {
    if(!clip)
        return false;
    stopPlayback();
    enableVideoPlayer();
    videoPlayer->openMedia(clip);
    return true;
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

void ViewerWidget::onScalingFinished(QPixmap *scaled) {
    imageViewer->updateFrame(scaled);
}

void ViewerWidget::closeImage() {
    imageViewer->closeImage();
    // TODO: implement this
    //videoPlayer->closeVideo();
}

void ViewerWidget::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setBrush(QBrush(bgColor));
    p.fillRect(this->rect(), p.brush());
}

void ViewerWidget::mouseMoveEvent(QMouseEvent *event) {
    event->ignore();
}
