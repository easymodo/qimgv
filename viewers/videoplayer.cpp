#include "videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent) :
    QWidget(parent),
    mediaPlayer(0, QMediaPlayer::VideoSurface),
    path("")
{
    QVideoWidget *videoWidget = new QVideoWidget;
    videoWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    this->setMouseTracking(true);

    mediaPlayer.setVideoOutput(videoWidget);
    readSettings();

    connect(&mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));
    connect(&mediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(handleMediaStatusChange(QMediaPlayer::MediaStatus)));
    connect(globalSettings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));

}

VideoPlayer::~VideoPlayer() {
}

void VideoPlayer::play(QString _path) {
    stop();
    path= _path;
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
}

void VideoPlayer::handleMediaStatusChange(QMediaPlayer::MediaStatus status) {
    if(status == QMediaPlayer::EndOfMedia) {
        play(path);
    }
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
    event->ignore();
}

