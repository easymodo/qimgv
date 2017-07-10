#ifndef THUMBNAILSTRIP_H
#define THUMBNAILSTRIP_H

#include <QLabel>
#include <QBoxLayout>
#include <QScrollArea>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QMutex>
#include <QTimer>
#include <QPainter>
#include <QResizeEvent>
#include <time.h>
#include "sourcecontainers/thumbnail.h"
#include "thumbnaillabel.h"
#include "thumbnailview.h"

class ThumbnailStrip : public QWidget
{
    Q_OBJECT
public:
    explicit ThumbnailStrip();
    ~ThumbnailStrip();

private:
    QList<ThumbnailLabel*> *thumbnailLabels;
    QHash<int, long> posIdHash;
    QHash<long, int> posIdHashReverse;
    void addThumbnailLabel();
    QBoxLayout *layout, *viewLayout;
    ThumbnailFrame thumbnailFrame;

    const int ANIMATION_SPEED_INSTANT = 0;
    const int ANIMATION_SPEED_FAST = 80;
    const int ANIMATION_SPEED_NORMAL = 150;

    const uint LOAD_DELAY = 40;
    const int OFFSCREEN_PRELOAD_AREA = 2500;

    int panelSize;
    int itemCount, current, thumbnailSize, thumbnailInterval;
    ThumbnailStrip *strip;
    QRectF preloadArea, visibleRegion;
    QTimer loadTimer;
    QScrollBar *scrollBar;
    PanelHPosition position;
    QMutex mutex;

    QGraphicsScene *scene;
    bool checkRange(int pos);
    long idCounter;
    void lock();
    void unlock();
    void updateThumbnailPositions(int start, int end);
    void updateSceneSize();
    void setThumbnailSize(int);
    void updateThumbnailSize();
    void ensureThumbnailVisible(int pos);
signals:
    void thumbnailRequested(QList<int>, int);
    void thumbnailClicked(int pos);

public slots:
    void onThumbnailClick(int pos);
    void populate(int count);
    void loadVisibleThumbnails();
    void loadVisibleThumbnailsDelayed();
    void setThumbnail(int, Thumbnail*);
    void fillPanel(int);
    void highlightThumbnail(int pos);
    void removeItemAt(int pos);
    void addItemAt(int pos);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);
};

#endif // THUMBNAILSTRIP_H
