/*
 * This widget combines ImageViewer / VideoPlayer.
 * Only one is displayed at a time.
 */

#include "viewerwidget.h"

ViewerWidget::ViewerWidget(ImageViewer *imageViewer, VideoPlayerGL *videoPlayer, QWidget *parent)
    : QWidget(parent),
      currentWidget(0)
{
    this->setMouseTracking(true);
    layout.setContentsMargins(0, 0, 0, 0);
    this->setLayout(&layout);
    setImageViewer(imageViewer);
    setVideoPlayer(videoPlayer);
    enableImageViewer();

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void ViewerWidget::setImageViewer(ImageViewer *imageViewer) {
    if(imageViewer) {
        this->imageViewer = imageViewer;
        imageViewer->setParent(this);
        imageViewer->hide();
    } else {
        qDebug() << "ViewerWidget: imageViewer is null";
    }
}

void ViewerWidget::setVideoPlayer(VideoPlayerGL *videoPlayer) {
    if(videoPlayer) {
        this->videoPlayer = videoPlayer;
        videoPlayer->setParent(this);
        videoPlayer->hide();
    } else {
        qDebug() << "ViewerWidget: videoPlayer is null";
    }
}

// hide videoPlayer, show imageViewer
void ViewerWidget::enableImageViewer() {
    if(currentWidget != 1) {
        if(currentWidget == 2) {
            videoPlayer->setPaused(true);
            videoPlayer->hide();
            layout.removeWidget(videoPlayer);
        }
        layout.addWidget(imageViewer);
        imageViewer->show();
        currentWidget = 1;
    }
}

// hide imageViewer, show videoPlayer
void ViewerWidget::enableVideoPlayer() {
    if(currentWidget != 2) {
        if(currentWidget == 1) {
            imageViewer->stopAnimation();
            imageViewer->hide();
            layout.removeWidget(imageViewer);
        }
        layout.addWidget(videoPlayer);
        videoPlayer->show();
        currentWidget = 2;
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
    if(currentWidget == 1)
        imageViewer->stopAnimation();
    if(currentWidget == 2)
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
