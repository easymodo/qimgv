#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QGraphicsLayoutItem>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QLinearGradient>
#include <QPropertyAnimation>
#include "thumbnail.h"

enum loadState { EMPTY, LOADING, LOADED };

class ThumbnailLabel : public QObject, public QGraphicsLayoutItem, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    ThumbnailLabel(QGraphicsPixmapItem *parent = 0);
    ~ThumbnailLabel();

    void setGeometry(const QRectF &rect) Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;

    bool isLoaded();
    loadState state;
    void setThumbnail(const Thumbnail *_thumbnail);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setHighlighted(bool x);
    bool isHighlighted();
    QColor *highlightColor, *outlineColor, *highlightColorBorder;
    void setOpacityAnimated(qreal amount, int speed);

private:
    const int SHADOW_HEIGHT = 10;
    bool hovered, loaded;
    const Thumbnail *thumbnail;
    int borderW, borderH, thumbnailSize;
    bool highlighted;
    QString infoString;
    QLinearGradient *shadowGradient;
    QRectF highlightRect, shadowRect, labelRect;
    bool showLabel;

protected:
 QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const Q_DECL_OVERRIDE;

 //void enterEvent(QEvent *event);
 //void leaveEvent(QEvent *event);
};

#endif // THUMBNAILLABEL_H

