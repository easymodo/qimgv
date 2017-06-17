#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QGraphicsItem>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QPropertyAnimation>
#include <ctime>
#include "sourcecontainers/thumbnail.h"
#include "settings.h"

enum loadState { EMPTY, LOADING, LOADED };

class ThumbnailLabel : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(qreal currentOpacity READ propertyOpacity WRITE propertySetOpacity)

public:
    ThumbnailLabel();
    ~ThumbnailLabel();

    loadState state;
    void setThumbnail(Thumbnail *_thumbnail);

    void setHighlighted(bool x, bool smooth);
    bool isHighlighted();
    void setOpacity(qreal amount, bool smooth);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void setLabelNum(int num);
    int labelNum();

    int width();
    int height();
    int getThumbnailSize();
    void setThumbnailSize(int size);

private:
    Thumbnail *thumbnail;
    int labelNumber;
    bool showLabel;
    qreal currentOpacity;
    bool highlighted, hovered;
    int thumbnailSize, highlightBarHeight, marginX;
    QString infoString;
    QRectF highlightBarRect, labelRect, nameRect, nameTextRect;
    QColor highlightColor, hoverHighlightColor, outlineColor, nameColor, labelColor;
    QFont font, fontSmall;
    QFontMetrics *fm, *fmSmall;
    QPixmap *loadingIcon;
    QPropertyAnimation *opacityAnimation;
    void updateLabelWidth();
    void setHovered(bool);
    bool isHovered();

    void propertySetOpacity(qreal amount);
    qreal propertyOpacity();

    const qreal inactiveOpacity = 0.83f;
    const int opacityAnimationSpeed = 80;

private slots:
    void readSettings();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

};

#endif // THUMBNAILLABEL_H

