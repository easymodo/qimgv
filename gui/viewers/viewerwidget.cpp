/*
 * This widget combines ImageViewer / VideoPlayer.
 * Only one is displayed at a time.
 */

#include "viewerwidget.h"

ViewerWidget::ViewerWidget(QWidget *parent)
    : QWidget(parent),
      imageViewer(NULL),
      videoPlayer(NULL),
      currentWidget(UNSET)
{
    this->setMouseTracking(true);
    layout.setContentsMargins(0, 0, 0, 0);
    this->setLayout(&layout);
    enableImageViewer();
    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

ImageViewer *ViewerWidget::getImageViewer() {
    if(!imageViewer) {
        initImageViewer();
    }
    return imageViewer;
}

VideoPlayerGL *ViewerWidget::getVideoPlayer() {
    if(!videoPlayer) {
        initVideoPlayer();
    }
    return videoPlayer;
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

void ViewerWidget::initImageViewer() {
    if(!imageViewer) {
        imageViewer = new ImageViewer();
        imageViewer->setParent(this);
        imageViewer->hide();
        connect(imageViewer, SIGNAL(scaleChanged(float)), this, SIGNAL(scaleChanged(float)));
        connect(imageViewer, SIGNAL(sourceSizeChanged(QSize)), this, SIGNAL(sourceSizeChanged(QSize)));
        connect(imageViewer, SIGNAL(imageAreaChanged(QRectF)), this, SIGNAL(imageAreaChanged(QRectF)));
    }
}

void ViewerWidget::initVideoPlayer() {
    if(!videoPlayer) {
        videoPlayer = new VideoPlayerGL();
        videoPlayer->setParent(this);
        videoPlayer->hide();
    }
}

// hide videoPlayer, show imageViewer
void ViewerWidget::enableImageViewer() {
    if(currentWidget != IMAGEVIEWER) {
        if(currentWidget == VIDEOPLAYER) {
            videoPlayer->setPaused(true);
            videoPlayer->hide();
            layout.removeWidget(videoPlayer);
        }
        if(!imageViewer) {
            initImageViewer();
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
            imageViewer->stopAnimation();
            imageViewer->hide();
            layout.removeWidget(imageViewer);
        }
        if(!videoPlayer)
            initVideoPlayer();
        layout.addWidget(videoPlayer);
        videoPlayer->show();
        currentWidget = VIDEOPLAYER;
    }
}

void ViewerWidget::readSettings() {
    bgColor = settings->backgroundColor();
    update();
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
    imageViewer->setFitMode(mode);
}

ImageFitMode ViewerWidget::fitMode() {
    return imageViewer->fitMode();
}

void ViewerWidget::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setBrush(QBrush(bgColor));
    p.fillRect(this->rect(), p.brush());
}

void ViewerWidget::mouseMoveEvent(QMouseEvent *event) {
    event->ignore();
}
