#include "videoplayermpv.h"
#include "mpvwidget.h"
#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QFileDialog>

// TODO: window flashes white when opening a video (straight from file manager)
VideoPlayerMpv::VideoPlayerMpv(QWidget *parent) : VideoPlayer(parent) {
    setAttribute(Qt::WA_TranslucentBackground, true);
    m_mpv = new MpvWidget(this);
    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0,0,0,0);
    vl->addWidget(m_mpv);
    setLayout(vl);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));

    qDebug() << "using mpv player";
}

bool VideoPlayerMpv::openMedia(Clip *clip) {
    if(clip) {
        QString file = clip->getPath();
        if (file.isEmpty())
            return false;
        m_mpv->command(QStringList() << "loadfile" << file);
        setPaused(false);
        //qDebug() << m_mpv->size() << this->devicePixelRatioF();
        return true;
    }
    return false;
}

void VideoPlayerMpv::seek(int pos) {
    m_mpv->command(QVariantList() << "seek" << pos << "absolute");
}

void VideoPlayerMpv::pauseResume() {
    const bool paused = m_mpv->getProperty("pause").toBool();
    setPaused(!paused);
}

void VideoPlayerMpv::setPaused(bool mode) {
    // TODO: ??????????? thats from QObject
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

}

void VideoPlayerMpv::readSettings() {
    setMuted(!settings->playVideoSounds());
    setVideoUnscaled(!settings->expandImage());
}
