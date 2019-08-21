#include "videoplayermpv.h"
#include "mpvwidget.h"
#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QFileDialog>

// TODO: window flashes white when opening a video (straight from file manager)
VideoPlayerMpv::VideoPlayerMpv(QWidget *parent) : VideoPlayer(parent) {
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);

    m_mpv = new MpvWidget(this);
    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0,0,0,0);
    vl->addWidget(m_mpv);
    setLayout(vl);

    readSettings();
    //connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
    connect(m_mpv, SIGNAL(durationChanged(int)), this, SIGNAL(durationChanged(int)));
    connect(m_mpv, SIGNAL(positionChanged(int)), this, SIGNAL(positionChanged(int)));
    connect(m_mpv, SIGNAL(videoPaused(bool)), this, SIGNAL(videoPaused(bool)));
}

bool VideoPlayerMpv::openMedia(QString file) {
    if(file.isEmpty())
        return false;
    m_mpv->command(QStringList() << "loadfile" << file);
    setPaused(false);
    //qDebug() << m_mpv->size() << this->devicePixelRatioF();
    return true;
}

void VideoPlayerMpv::seek(int pos) {
    m_mpv->command(QVariantList() << "seek" << pos << "absolute");
    //qDebug() << "seek(): " << pos << " sec";
}

void VideoPlayerMpv::seekRelative(int pos) {
    m_mpv->command(QVariantList() << "seek" << pos << "relative");
    //qDebug() << "seekRelative(): " << pos << " sec";
}

void VideoPlayerMpv::pauseResume() {
    const bool paused = m_mpv->getProperty("pause").toBool();
    setPaused(!paused);
}

void VideoPlayerMpv::frameStep() {
    m_mpv->command(QVariantList() << "frame-step");
}

void VideoPlayerMpv::frameStepBack() {
    m_mpv->command(QVariantList() << "frame-back-step");
}

void VideoPlayerMpv::stop() {
    m_mpv->command(QVariantList() << "stop");
}

void VideoPlayerMpv::setPaused(bool mode) {
    m_mpv->setProperty("pause", mode);
}

void VideoPlayerMpv::setMuted(bool mode) {
    m_mpv->setMuted(mode);
}

void VideoPlayerMpv::setVideoUnscaled(bool mode) {
    if(mode)
        m_mpv->setOption("video-unscaled", "downscale-big");
    else
        m_mpv->setOption("video-unscaled", "no");
}

void VideoPlayerMpv::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
}

void VideoPlayerMpv::readSettings() {
    //setMuted(!settings->playVideoSounds());
    //setVideoUnscaled(!settings->expandImage());
}

void VideoPlayerMpv::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    event->ignore();
    if(event->button() == Qt::LeftButton)
        this->pauseResume();
}

void VideoPlayerMpv::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    event->ignore();
}

void VideoPlayerMpv::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    event->ignore();
}

void VideoPlayerMpv::keyPressEvent(QKeyEvent *event) {
    qDebug() << event->key();
    if(event->key() == Qt::Key_Space) {
        event->accept();
        pauseResume();
    } else {
        event->ignore();
    }
}

void VideoPlayerMpv::show() {
    QWidget::show();
    this->setFocus();
}

void VideoPlayerMpv::hide() {
    this->clearFocus();
    QWidget::hide();
}

VideoPlayer *CreatePlayerWidget() {
    return new VideoPlayerMpv();
}
