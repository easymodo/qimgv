#ifndef THUMBNAILSTRIP_H
#define THUMBNAILSTRIP_H

#include <QLabel>
#include <QBoxLayout>
#include <QScrollArea>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QTimer>
#include <QTimeLine>
#include <QPropertyAnimation>
#include "../customWidgets/clickablelabel.h"
#include "../customWidgets/clickablewidget.h"
#include "../sourceContainers/thumbnail.h"
#include "thumbnaillabel.h"
#include "thumbnailview.h"
#include <time.h>

class ThumbnailStrip : public QWidget
{
    Q_OBJECT
public:
    explicit ThumbnailStrip(QWidget *parent = 0);
    ~ThumbnailStrip();

    void updatePanelPosition();
private:
    QList<ThumbnailLabel*> *thumbnailLabels;
    void addItem();
    //QGraphicsLinearLayout *viewLayout;
    QBoxLayout *layout, *buttonsLayout, *viewLayout;
    QWidget *buttonsWidget;
    ClickableLabel *openButton, *saveButton, *settingsButton, *exitButton;
    ClickableWidget *widget;
    ThumbnailView *thumbView;
    //CustomScene* scene;
    QTimeLine *timeLine;

    const qreal OPACITY_INACTIVE = 0.75;
    const qreal OPACITY_SELECTED = 1.0;
    const int ANIMATION_SPEED_INSTANT = 0;
    const int ANIMATION_SPEED_FAST = 80;
    const int ANIMATION_SPEED_NORMAL = 150;

    const int SCROLL_UPDATE_RATE = 8;
    const float SCROLL_SPEED_MULTIPLIER = 3.1;
    const int SCROLL_ANIMATION_SPEED = 100;
    const uint LOAD_DELAY = 20;
    const int OFFSCREEN_PRELOAD_AREA = 1500;

    int panelSize;
    int itemCount, current, thumbnailSize;
    bool childVisible(int pos);
    ThumbnailStrip *strip;
    QRectF preloadArea, visibleRegion;
    QTimer loadTimer;
    bool childVisibleEntirely(int pos);
    //QRectF itemsBoundingRect();
    QScrollBar *scrollBar;
    PanelPosition position;
    QSize parentSz;

    void requestThumbnail(int pos);
signals:
    void thumbnailRequested(int pos);
    void thumbnailClicked(int pos);
    void openClicked();
    void saveClicked();
    void settingsClicked();
    void exitClicked();
    void panelSizeChanged();

public slots:
    void populate(int count);
    void parentResized(QSize parentSize);
    void loadVisibleThumbnails();
    void loadVisibleThumbnailsDelayed();
    void setThumbnail(int, Thumbnail*);
    void fillPanel(int);
    void selectThumbnail(int pos);

protected:
    void wheelEvent(QWheelEvent *event);
    void leaveEvent(QEvent *event);

private slots:
    void viewPressed(QPoint pos);
    void updateVisibleRegion();
    void readSettings();
};

#endif // THUMBNAILSTRIP_H
