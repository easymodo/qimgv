#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QPropertyAnimation>
#include <QPaintEngine>
#include <cmath>
#include "sourcecontainers/thumbnail.h"
#include "settings.h"
#include "sharedresources.h"

enum LoadState {
    EMPTY,
    LOADING,
    LOADED
};
enum HighlightStyle {
    HIGHLIGHT_TOPBAR,
    HIGHLIGHT_BACKGROUND
};

class ThumbnailWidget : public QGraphicsWidget {
    Q_OBJECT
    Q_PROPERTY(qreal currentOpacity READ propertyOpacity WRITE propertySetOpacity)

public:
    ThumbnailWidget(QGraphicsItem *parent = nullptr);
    ~ThumbnailWidget();

    LoadState state;
    void setThumbnail(std::shared_ptr<Thumbnail> _thumbnail);

    void setHighlighted(bool x, bool smooth);
    bool isHighlighted();
    void setOpacity(qreal amount, bool smooth);

    QRectF boundingRect() const Q_DECL_OVERRIDE;

    int width();
    int height();
    void setThumbnailSize(int size);

    void setGeometry(const QRectF &rect);

    virtual QRectF geometry() const;
    QSizeF effectiveSizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void setDrawLabel(bool mode);
    void setHightlightStyle(HighlightStyle style);
    void setMargins(int x, int y);
private:
    std::shared_ptr<Thumbnail> thumbnail;
    qreal currentOpacity;
    HighlightStyle highlightStyle;
    bool highlighted, hovered, mDrawLabel;
    int thumbnailSize, marginY, marginX;
    QRectF highlightBarRect, nameRect, nameTextRect, labelTextRect;
    QColor highlightColor, nameColor;
    QFont font, fontSmall;
    QFontMetrics *fm, *fmSmall;
    QPropertyAnimation *opacityAnimation;
    void setHovered(bool);
    bool isHovered();

    void propertySetOpacity(qreal amount);
    qreal propertyOpacity();

    const qreal inactiveOpacity = 0.86f;
    const int opacityAnimationSpeed = 80;
    void setupLabel();
    void drawThumbnail(QPainter* painter, qreal dpr, const QPixmap *pixmap);
    void drawIcon(QPainter *painter, qreal dpr, const QPixmap *pixmap);

    void drawHighlight(QPainter *painter);
    void drawLabel(QPainter *painter);
private slots:
    void readSettings();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

    void updateGeometry();
};

#endif // THUMBNAILWIDGET_H

