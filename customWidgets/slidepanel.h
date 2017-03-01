#ifndef SLIDEPANEL_H
#define SLIDEPANEL_H

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

class SlidePanel : public QWidget
{
    Q_OBJECT
public:
    explicit SlidePanel(QWidget *w, QWidget *parent = 0);
    ~SlidePanel();
    virtual void updatePanelPosition() = 0;
    void setPosition(PanelPosition);

private:
    void setWidget(QWidget* w);
    QBoxLayout *layout;

signals:
    void panelSizeChanged();

public slots:
    virtual void parentResized(QSize parentSize) = 0;
    void show();

protected:
    QGraphicsOpacityEffect *fadeEffect;
    QPropertyAnimation *fadeAnimation, *slideAnimation;
    QParallelAnimationGroup *animGroup;
    int panelSize;
    PanelPosition position;
    QSize parentSz;
    bool parentFullscreen;
    QWidget *mWidget;
    QPoint initialPosition;
    void leaveEvent(QEvent *event);
};

#endif // SLIDEPANEL_H
