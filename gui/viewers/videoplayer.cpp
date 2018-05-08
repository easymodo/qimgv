#include "videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFocusPolicy(Qt::NoFocus);
}

void VideoPlayer::show() {
    QWidget::show();
}

void VideoPlayer::hide() {
    QWidget::hide();
}
