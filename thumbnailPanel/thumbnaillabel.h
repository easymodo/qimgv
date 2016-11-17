#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QGraphicsItem>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QPropertyAnimation>
#include <ctime>
#include "../sourceContainers/thumbnail.h"
#include "../settings.h"

enum loadState { EMPTY, LOADING, LOADED };

class ThumbnailLabel : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(qreal currentOpacity READ opacity WRITE setOpacity)

public:
    ThumbnailLabel();
    ~ThumbnailLabel();

    bool isLoaded();
    loadState state;
    void setThumbnail(Thumbnail *_thumbnail);

    void setHighlighted(bool x);
    bool isHighlighted();
    void setOpacity(qreal amount);
    qreal opacity();
    void readSettings();
    void setOpacityAnimated(qreal amount, int speed);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void setLabelNum(int num);
    int labelNum();

    int width();
    int height();
    int getThumbnailSize();
private:
    int labelNumber;
    bool loaded;
    bool showLabel, showName;
    bool drawSelectionBorder;

    Thumbnail *thumbnail;
    qreal currentOpacity;
    bool highlighted;
    int borderW, borderH, thumbnailSize;
    QString infoString;
    QRectF highlightRect, labelRect, nameRect;
    QColor *highlightColor, *outlineColor, *nameColor, *labelColor;
    QFont font;
    QFontMetrics *fm;
    void updateLabelWidth();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

};

#endif // THUMBNAILLABEL_H

