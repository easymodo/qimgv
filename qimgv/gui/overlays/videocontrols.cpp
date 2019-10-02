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

    lastVideoPosition = -1;

    readSettings();
    connect(settings, &Settings::settingsChanged, this, &VideoControls::readSettings);

    connect(ui->pauseButton, &QPushButton::pressed, this, &VideoControls::pause);
    connect(ui->seekBar, &VideoSlider::sliderMovedX, this, &VideoControls::seek);
    connect(ui->prevFrameButton, &QPushButton::pressed, this, &VideoControls::prevFrame);
    connect(ui->nextFrameButton, &QPushButton::pressed, this, &VideoControls::nextFrame);

    if(parent)
        setContainerSize(parent->size());
}

void VideoControls::readSettings() {
    if(settings->panelEnabled() && settings->panelPosition() == PanelHPosition::PANEL_BOTTOM)
        setPosition(FloatingWidgetPosition::TOP);
    else
        setPosition(FloatingWidgetPosition::BOTTOM);
}

VideoControls::~VideoControls() {
    delete ui;
}

void VideoControls::setDurationSeconds(int time) {
    int _time = time;
    int hours   = _time / 3600;
    _time -= hours * 3600;
    int minutes = _time / 60;
    int seconds = _time - minutes * 60;
    //qDebug() << "total:" << hours << ":" << minutes << ":" << seconds;
    QString str = QString("%1").arg(minutes, 2, 10, QChar('0')) + ":" +
                  QString("%1").arg(seconds, 2, 10, QChar('0'));
    if(hours)
        str.prepend(QString("%1").arg(hours, 2, 10, QChar('0')) + ":");
    ui->seekBar->setRange(0, time);
    ui->durationLabel->setText(str);
    recalculateGeometry();
}

void VideoControls::setPositionSeconds(int time) {
    if(time != lastVideoPosition) {
        int _time = time;
        int hours   = _time / 3600;
        _time -= hours * 3600;
        int minutes = _time / 60;
        int seconds = _time - minutes * 60;
        //qDebug() << hours << ":" << minutes << ":" << seconds;
        QString str = QString("%1").arg(minutes, 2, 10, QChar('0')) + ":" +
                      QString("%1").arg(seconds, 2, 10, QChar('0'));
        if(hours)
            str.prepend(QString("%1").arg(hours, 2, 10, QChar('0')) + ":");

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
