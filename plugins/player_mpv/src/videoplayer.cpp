#include "videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::NoFocus);
}

void VideoPlayer::show() {
    QWidget::show();
}

void VideoPlayer::hide() {
    QWidget::hide();
}
