#include "videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent) :
    QGraphicsView(parent),
    mediaPlayer(0, QMediaPlayer::VideoSurface),
    path("") {
    scene = new QGraphicsScene;
    videoItem = new QGraphicsVideoItem();
    mediaPlayer.setVideoOutput(videoItem);
    retries = 1;

    scene->addItem(videoItem);
    this->setRenderHint(QPainter::SmoothPixmapTransform);
    videoItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setAcceptDrops(false);
    this->setScene(scene);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //this->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setFrameShape(QFrame::NoFrame);

    readSettings();

    connect(&mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));
    connect(&mediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(handleMediaStatusChange(QMediaPlayer::MediaStatus)));
    connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(handlePlayerStateChange(QMediaPlayer::State)));
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
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
    } else {
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
    mediaPlayer.setMuted(!settings->playVideoSounds());
    QBrush brush(settings->backgroundColor());
    this->setBackgroundBrush(brush);
}

void VideoPlayer::handleMediaStatusChange(QMediaPlayer::MediaStatus status) {
    if(status == QMediaPlayer::EndOfMedia) {
        play(path);
    } else if(status == QMediaPlayer::BufferedMedia) {
        adjustVideoSize();
    }
}

//fits && centers video in window
void VideoPlayer::adjustVideoSize() {
    QSize size = videoItem->nativeSize().toSize();
    if(size.width() > this->width() || size.height() > this->height()) {
        size = size.scaled(this->width(), this->height(), Qt::KeepAspectRatio);
    } else {
        size = videoItem->nativeSize().toSize();
    }
    videoItem->setSize(size);
    scene->setSceneRect(scene->itemsBoundingRect());
}

void VideoPlayer::handlePlayerStateChange(QMediaPlayer::State state) {
    Q_UNUSED(state)
}

// Try reloading video if it fails
void VideoPlayer::handleError() {
    qDebug() << "VideoPlayer: Error - " + mediaPlayer.errorString();
    while (retries>0){
    play(path);
    retries--;
    }
}

void VideoPlayer::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    //event->ignore();
}

void VideoPlayer::mousePressEvent(QMouseEvent *event) {
    //QWidget::mousePressEvent(event);
    event->ignore();
}

void VideoPlayer::mouseReleaseEvent(QMouseEvent *event) {
    event->ignore();
}

void VideoPlayer::wheelEvent(QWheelEvent *event) {
    event->ignore();
}
