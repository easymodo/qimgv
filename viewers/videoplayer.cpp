#include "videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent) :
    QGraphicsView(parent),
    mediaPlayer(0, QMediaPlayer::VideoSurface),
    path("")
{
    scene = new CustomScene;
    videoItem = new QGraphicsVideoItem();
    mediaPlayer.setVideoOutput(videoItem);

    scene->addItem(videoItem);
    videoItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setMouseTracking(true);
    this->setScene(scene);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setFrameShape(QFrame::NoFrame);

    readSettings();

    connect(&mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));
    connect(&mediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(handleMediaStatusChange(QMediaPlayer::MediaStatus)));
    connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(handlePlayerStateChange(QMediaPlayer::State)));
    connect(globalSettings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
    connect(this, SIGNAL(parentResized(QSize)), this, SLOT(adjustVideoSize()));
    connect(videoItem, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(adjustVideoSize()));

}

VideoPlayer::~VideoPlayer() {
}

void VideoPlayer::play(QString _path) {
    stop();
    path = _path;
    if(!path.isEmpty()) {
        mediaPlayer.setMedia(QUrl::fromLocalFile(path));
        switch(mediaPlayer.state()) {
        case QMediaPlayer::PlayingState:
            mediaPlayer.pause();
            break;
        default:
            mediaPlayer.play();
            break;
        }
    }
    else {
        qDebug() << "VideoPlayer: empty path.";
    }
}

void VideoPlayer::replay() {
    play(path);
}

void VideoPlayer::stop() {
    mediaPlayer.stop();
}

void VideoPlayer::readSettings() {
    mediaPlayer.setMuted(!globalSettings->playVideoSounds());
    QBrush brush(globalSettings->backgroundColor());
    this->setBackgroundBrush(brush);
}

void VideoPlayer::handleMediaStatusChange(QMediaPlayer::MediaStatus status) {
    if(status == QMediaPlayer::EndOfMedia) {
        play(path);
    } else if(status == QMediaPlayer::BufferedMedia) {
        adjustVideoSize();
    }
}

//fits entire video in window
void VideoPlayer::adjustVideoSize() {
    QSize size = videoItem->nativeSize().toSize();
    if(size.width() > this->width() || size.height() > this->height()) {
        size = size.boundedTo(this->size());
        videoItem->setSize(size);
    } else if(size != videoItem->size()) {
        videoItem->setSize(videoItem->nativeSize());
    }
    scene->setSceneRect(scene->itemsBoundingRect());
}

void VideoPlayer::handlePlayerStateChange(QMediaPlayer::State state) {
}

void VideoPlayer::handleError() {
    qDebug() << "VideoPlayer: Error - " + mediaPlayer.errorString();
}

void VideoPlayer::mouseDoubleClickEvent(QMouseEvent *event) {
    QWidget::mouseDoubleClickEvent(event);
    if(event->button() == Qt::RightButton) {
        //emit sendRightDoubleClick();
    }
    else {
        emit sendDoubleClick();
    }
}
void VideoPlayer::mouseMoveEvent(QMouseEvent* event) {
    QWidget::mouseMoveEvent(event);
    //event->ignore();
}

void VideoPlayer::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
}
