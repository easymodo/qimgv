#include "videocontrols.h"
#include "ui_videocontrols.h"

VideoControls::VideoControls(OverlayContainerWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::VideoControls)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_NoMousePropagation, true);
    hide();

    playIcon.addPixmap(QPixmap(":res/icons/buttons/play24.png"));
    pauseIcon.addPixmap(QPixmap(":res/icons/buttons/pause24.png"));

    setPosition(FloatingWidgetPosition::BOTTOM);

    lastVideoPosition = -1;

    //ui->seekLeftButton->setVisible(false);
    //ui->seekRightButton->setVisible(false);

    connect(ui->pauseButton, SIGNAL(pressed()), this, SIGNAL(pause()));
    //connect(ui->seekLeftButton, SIGNAL(pressed()), this, SIGNAL(seekLeft()));
    //connect(ui->seekRightButton, SIGNAL(pressed()), this, SIGNAL(seekRight()));
    connect(ui->prevFrameButton, SIGNAL(pressed()), this, SIGNAL(prevFrame()));
    connect(ui->nextFrameButton, SIGNAL(pressed()), this, SIGNAL(nextFrame()));
    connect(ui->seekBar, SIGNAL(sliderMovedX(int)), this, SIGNAL(seek(int)));

    if(parent)
        setContainerSize(parent->size());
}

VideoControls::~VideoControls() {
    delete ui;
}

void VideoControls::setDurationSeconds(int time) {
    int hours   = time / 3600;
    int minutes = time / 60;
    int seconds = time % 60;
    //qDebug() << "total:" << hours << ":" << minutes << ":" << seconds;
    QString str = QString("%1").arg(minutes, 2, 10, QChar('0')) + ":" +
                  QString("%1").arg(seconds, 2, 10, QChar('0'));
    if(hours)
        str.append(QString("%1").arg(hours, 2, 10, QChar('0')) + ":");

    ui->seekBar->setRange(0, time);
    ui->durationLabel->setText(str);
    recalculateGeometry();
}

void VideoControls::setPositionSeconds(int time) {
    if(time != lastVideoPosition) {
        int hours   = time / 3600;
        int minutes = time / 60;
        int seconds = time % 60;
        //qDebug() << hours << ":" << minutes << ":" << seconds;
        QString str = QString("%1").arg(minutes, 2, 10, QChar('0')) + ":" +
                      QString("%1").arg(seconds, 2, 10, QChar('0'));
        if(hours)
            str.append(QString("%1").arg(hours, 2, 10, QChar('0')) + ":");

        ui->positionLabel->setText(str);
        ui->seekBar->blockSignals(true);
        ui->seekBar->setValue(time);
        ui->seekBar->blockSignals(false);
        recalculateGeometry();
    }
    lastVideoPosition = time;
}

void VideoControls::onVideoPaused(bool mode) {
    if(mode)
        ui->pauseButton->setIcon(playIcon);
    else
        ui->pauseButton->setIcon(pauseIcon);
}
