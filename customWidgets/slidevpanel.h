#ifndef SLIDEVPANEL_H
#define SLIDEVPANEL_H

#include <QWidget>
#include <QTimeLine>
#include <QGraphicsScene>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPainter>
#include <QBoxLayout>
#include <QPushButton>
#include "../settings.h"
#include <QDebug>

class SlideVPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SlideVPanel(QWidget *w, QWidget *parent = 0);
    ~SlideVPanel();
    void updatePanelPosition();

private:
    void setWidget(QWidget* w);
    QGraphicsOpacityEffect *fadeEffect;
    QPropertyAnimation *fadeAnimation, *slideAnimation;
    QParallelAnimationGroup *animGroup;

    int panelSize;
    PanelPosition position;
    QSize parentSz;
    bool parentFullscreen;
    QWidget *mWidget;
    QBoxLayout *layout;

signals:
    void panelSizeChanged();

public slots:
    void parentResized(QSize parentSize);
    void show();

protected:
    void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent* event);
};

#endif // SLIDEVPANEL_H
