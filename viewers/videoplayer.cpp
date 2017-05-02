#include "videoplayer.h"
/*
// todo: just use mpv. QMediaPlayer can't into gapless loops.
// todo: test switching webm -> gif. seems buggy
VideoPlayer::VideoPlayer(QWidget *parent) : QGraphicsView(parent),
    mediaPlayer(0, QMediaPlayer::VideoSurface) {
    clip = new Clip();
    scene = new QGraphicsScene;
    textMessage = new QGraphicsSimpleTextItem();
    textMessage->setPen(QPen(QColor(Qt::white)));
    videoItem = new QGraphicsVideoItem();
    mediaPlayer.setVideoOutput(videoItem);
    mediaPlaylist.setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    mediaPlayer.setPlaylist(&mediaPlaylist);

    scene->addItem(videoItem);
    this->setRenderHint(QPainter::SmoothPixmapTransform);
    videoItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setAcceptDrops(false);
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
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
    connect(this, SIGNAL(parentResized(QSize)), this, SLOT(adjustVideoSize()));
    connect(videoItem, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(adjustVideoSize()));

}

VideoPlayer::~VideoPlayer() {
    delete clip;
}

// display & initialize
void VideoPlayer::displayVideo(Clip *_clip) {
    delete clip;
    clip = new Clip(*_clip);
    transformVideo();
    play();
}

void VideoPlayer::play() {
    stop();    
    QString path = clip->getPath();
    if(!path.isEmpty()) {
        mediaPlaylist.clear();
        mediaPlaylist.addMedia(QUrl::fromLocalFile(path));
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

void VideoPlayer::stop() {
    mediaPlayer.stop();
}

void VideoPlayer::readSettings() {
    mediaPlayer.setMuted(!settings->playVideoSounds());
    QBrush brush(settings->backgroundColor());
    this->setBackgroundBrush(brush);
}

void VideoPlayer::handleMediaStatusChange(QMediaPlayer::MediaStatus status) {
    if(status == QMediaPlayer::BufferedMedia) {
        adjustVideoSize();
    }
}

//fits && centers video in window
void VideoPlayer::adjustVideoSize() {
    QSize size = clip->size();
    if(size.width() > this->width() || size.height() > this->height()) {
        size = size.scaled(this->width(), this->height(), Qt::KeepAspectRatio);
    } else {
        size = clip->size();
    }
    videoItem->setSize(size);
    scene->setSceneRect(scene->itemsBoundingRect());
}

void VideoPlayer::transformVideo() {
    videoItem->setTransform(clip->getTransform());
}

void VideoPlayer::handlePlayerStateChange(QMediaPlayer::State state) {
    Q_UNUSED(state)
}

// Try reloading video if it fails
void VideoPlayer::handleError() {
    qDebug() << "VideoPlayer: Error - " + mediaPlayer.errorString();
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
*/
