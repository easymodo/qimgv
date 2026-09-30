#pragma once

#include <QWidget>
#include <QBoxLayout>
#include <QPainter>

/* Wraps a viewer so it can be given a focus frame in split view.
 * The frame is drawn inside the widget margins, so the child widget
 * (and the image it fits into it) never ends up underneath it.
 */
class SplitPane : public QWidget {
    Q_OBJECT
public:
    explicit SplitPane(QWidget *content, QWidget *parent = nullptr);
    void setFrameVisible(bool mode);
    void setActive(bool mode);

    static const int FRAME_WIDTH = 4;

protected:
    void paintEvent(QPaintEvent *event);

private:
    QBoxLayout *mLayout;
    bool mFrameVisible, mActive;
};
