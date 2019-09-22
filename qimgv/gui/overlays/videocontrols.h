#ifndef VIDEOCONTROLS_H
#define VIDEOCONTROLS_H

#include "gui/customwidgets/floatingwidget.h"
#include "settings.h"
#include <QPushButton>

namespace Ui {
class VideoControls;
}

class VideoControls : public FloatingWidget
{
    Q_OBJECT

public:
    explicit VideoControls(OverlayContainerWidget *parent = nullptr);
    ~VideoControls();

public slots:
    void setDurationSeconds(int);
    void setPositionSeconds(int);
    void onVideoPaused(bool);

signals:
    void pause();
    void seek(int pos);
    void seekRight();
    void seekLeft();
    void nextFrame();
    void prevFrame();

private slots:
    void readSettings();

private:
    Ui::VideoControls *ui;
    int lastVideoPosition;
    QIcon playIcon, pauseIcon;
};

#endif // VIDEOCONTROLS_H
