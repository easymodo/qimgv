#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QGraphicsLayoutItem>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

enum loadState { EMPTY, LOADING, LOADED };

class ThumbnailLabel : public QGraphicsLayoutItem, public QGraphicsPixmapItem
{
public:
    ThumbnailLabel(QGraphicsPixmapItem *parent = 0);
    ~ThumbnailLabel();

    void setGeometry(const QRectF &rect) Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;

    bool isLoaded();
    loadState state;
    void setPixmap(const QPixmap &pixmap);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setHighlighted(bool x);
    bool isHighlighted();
    QColor *highlightColor, *outlineColor;
private:
    bool hovered, loaded;
    QPixmap* pix;
    int borderW, borderH;
    bool highlighted;

protected:
 QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const Q_DECL_OVERRIDE;

 //void enterEvent(QEvent *event);
 //void leaveEvent(QEvent *event);
};

#endif // THUMBNAILLABEL_H
