#include "videoplayergl.h"
#include "mpvwidget.h"
#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QFileDialog>

VideoPlayerGL::VideoPlayerGL(QWidget *parent) : QWidget(parent) {
/*    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFocusPolicy(Qt::NoFocus);

    m_mpv = new MpvWidget(this);
    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0,0,0,0);
    vl->addWidget(m_mpv);
    setLayout(vl);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));

    */
}

bool VideoPlayerGL::openMedia(Clip *clip) {
    /*if(clip) {
        QString file = clip->getPath();
        if (file.isEmpty())
            return false;
        m_mpv->command(QStringList() << "loadfile" << file);
        setPaused(false);
        return true;
    }
    return false;
    */
}

void VideoPlayerGL::seek(int pos) {
//    m_mpv->command(QVariantList() << "seek" << pos << "absolute");
}

void VideoPlayerGL::pauseResume() {
//    const bool paused = m_mpv->getProperty("pause").toBool();
//    setPaused(!paused);
}

void VideoPlayerGL::setPaused(bool mode) {
    // TODO: ??????????? thats from QObject
//    m_mpv->setProperty("pause", mode);
}

void VideoPlayerGL::setMuted(bool mode) {
//    m_mpv->setMuted(mode);
}

void VideoPlayerGL::readSettings() {
//    setMuted(!settings->playVideoSounds());
}
