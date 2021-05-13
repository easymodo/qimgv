#include "videoplayerinitproxy.h"

#ifdef _QIMGV_PLAYER_PLUGIN
    #define QIMGV_PLAYER_PLUGIN _QIMGV_PLAYER_PLUGIN
#else
    #define QIMGV_PLAYER_PLUGIN ""
#endif
#ifdef _QIMGV_PLUGIN_DIR
    #define QIMGV_PLUGIN_DIR _QIMGV_PLUGIN_DIR
#else
    #define QIMGV_PLUGIN_DIR ""
#endif

VideoPlayerInitProxy::VideoPlayerInitProxy(QWidget *parent)
    : VideoPlayer(parent),
      player(nullptr)
{
    setAccessibleName("VideoPlayerInitProxy");
    setMouseTracking(true);
    layout.setContentsMargins(0,0,0,0);
    setLayout(&layout);
    connect(settings, &Settings::settingsChanged, this, &VideoPlayerInitProxy::onSettingsChanged);

    libDirDefault  = QIMGV_PLUGIN_DIR;
    libFile = QIMGV_PLAYER_PLUGIN;

#ifdef _WIN32
    // windows builds are portable, so libDirDefault is just a directory name
    // (cmake does not know the full path)
    libDirs << QApplication::applicationDirPath() + "/" + libDirDefault;
#else
    libDirs << libDirDefault << "/usr/lib/qimgv" << "/usr/lib64/qimgv";
#endif
}

VideoPlayerInitProxy::~VideoPlayerInitProxy() {
}

void VideoPlayerInitProxy::onSettingsChanged() {
    if(!player)
        return;
    player->setMuted(!settings->playVideoSounds());
    player->setVideoUnscaled(!settings->expandImage());
}

std::shared_ptr<VideoPlayer> VideoPlayerInitProxy::getPlayer() {
    return player;
}

bool VideoPlayerInitProxy::isInitialized() {
    return (player != nullptr);
}

inline bool VideoPlayerInitProxy::initPlayer() {
#ifndef USE_MPV
    return false;
#endif
    if(player)
        return true;

    QFileInfo pluginFile;
    for(auto dir : libDirs) {
        pluginFile.setFile(dir + "/" + libFile);
        if(pluginFile.isFile() && pluginFile.isReadable()) {
            playerLib.setFileName(pluginFile.absoluteFilePath());
            break;
        }
    }
    if(playerLib.fileName().isEmpty()) {
        qDebug() << "Could not find" << libFile << "in the following directories:" << libDirs;
        return false;
    }

// load lib
    typedef VideoPlayer* (*createPlayerWidgetFn)();
    createPlayerWidgetFn fn = (createPlayerWidgetFn) playerLib.resolve("CreatePlayerWidget");
    if(fn) {
        VideoPlayer* pl = fn();
        player.reset(pl);
    }
    if(!player) {
        qDebug() << "Could not load:" << playerLib.fileName() << ". Wrong plugin version?";
        return false;
    }

    player->setMuted(!settings->playVideoSounds());
    player->setVideoUnscaled(!settings->expandImage());
    player->setVolume(settings->volume());

    player->setParent(this);
    layout.addWidget(player.get());
    player->hide();
    setFocusProxy(player.get());
    connect(player.get(), SIGNAL(durationChanged(int)), this, SIGNAL(durationChanged(int)));
    connect(player.get(), SIGNAL(positionChanged(int)), this, SIGNAL(positionChanged(int)));
    connect(player.get(), SIGNAL(videoPaused(bool)),    this, SIGNAL(videoPaused(bool)));
    connect(player.get(), SIGNAL(playbackFinished()),   this, SIGNAL(playbackFinished()));
    return true;
}

bool VideoPlayerInitProxy::openMedia(QString file) {
    if(!initPlayer())
        return false;
    return player->openMedia(file);
}

void VideoPlayerInitProxy::seek(int pos) {
    if(!player)
        return;
    player->seek(pos);
}

void VideoPlayerInitProxy::seekRelative(int pos) {
    if(!player)
        return;
    player->seekRelative(pos);
}

void VideoPlayerInitProxy::pauseResume() {
    if(!player)
        return;
    player->pauseResume();
}

void VideoPlayerInitProxy::frameStep() {
    if(!player)
        return;
    player->frameStep();
}

void VideoPlayerInitProxy::frameStepBack() {
    if(!player)
        return;
    player->frameStepBack();
}

void VideoPlayerInitProxy::stop() {
    if(!player)
        return;
    player->stop();
}

void VideoPlayerInitProxy::setPaused(bool mode) {
    if(!player)
        return;
    player->setPaused(mode);
}

void VideoPlayerInitProxy::setMuted(bool mode) {
    if(!player)
        return;
    player->setMuted(mode);
}

bool VideoPlayerInitProxy::muted() {
    if(!player)
        return true;
    return player->muted();
}

void VideoPlayerInitProxy::volumeUp() {
    if(!player)
        return;
    player->volumeUp();
    settings->setVolume(player->volume());
}

void VideoPlayerInitProxy::volumeDown() {
    if(!player)
        return;
    player->volumeDown();
    settings->setVolume(player->volume());
}

void VideoPlayerInitProxy::setVolume(int vol) {
    if(!player)
        return;
    player->setVolume(vol);
}

int VideoPlayerInitProxy::volume() {
    if(!player)
        return 0;
    return player->volume();
}

void VideoPlayerInitProxy::setVideoUnscaled(bool mode) {
    if(!player)
        return;
    player->setVideoUnscaled(mode);
}

void VideoPlayerInitProxy::setLoopPlayback(bool mode) {
    if(!player)
        return;
    player->setLoopPlayback(mode);
}

void VideoPlayerInitProxy::show() {
    if(initPlayer()) {
        layout.removeWidget(errorLabel);
        player->show();
    } else if(!errorLabel) {
        errorLabel = new QLabel(this);
        errorLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
        errorLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        //errorLabel->setAlignment(Qt::AlignVCenter);
        QString errString = "Could not load " + libFile + " from:";
        for(auto path : libDirs)
            errString.append("\n" + path + "/");
        errorLabel->setText(errString);
        layout.addWidget(errorLabel);
    }
    VideoPlayer::show();
}

void VideoPlayerInitProxy::hide() {
    if(player)
        player->hide();
    VideoPlayer::hide();
}

void VideoPlayerInitProxy::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
}
