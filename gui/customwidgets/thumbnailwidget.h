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

class ThumbnailWidget : public QGraphicsWidget {
    Q_OBJECT
    Q_PROPERTY(qreal currentOpacity READ propertyOpacity WRITE propertySetOpacity)

public:
    ThumbnailWidget(QGraphicsItem *parent = nullptr);
    ~ThumbnailWidget() Q_DECL_OVERRIDE;

    LoadState state;
    void setThumbnail(std::shared_ptr<Thumbnail> _thumbnail);

    void setHighlighted(bool x, bool smooth);
    bool isHighlighted();
    void setOpacity(qreal amount, bool smooth);

    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

    qreal width();
    qreal height();
    void setThumbnailSize(int size);

    void setGeometry(const QRectF &rect) Q_DECL_OVERRIDE;

    virtual QRectF geometry() const;
    QSizeF effectiveSizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void setDrawLabel(bool mode);
    void setMargins(int x, int y);
private:
    void setHovered(bool);
    bool isHovered();

    void propertySetOpacity(qreal amount);
    qreal propertyOpacity();


private slots:
    void readSettings();

protected:
    virtual void setupLayout();
    virtual void drawThumbnail(QPainter* painter, qreal dpr, const QPixmap *pixmap);
    virtual void drawIcon(QPainter *painter, qreal dpr, const QPixmap *pixmap);
    virtual void drawHighlight(QPainter *painter);
    virtual void drawLabel(QPainter *painter);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const Q_DECL_OVERRIDE;
    void updateGeometry() Q_DECL_OVERRIDE;

    std::shared_ptr<Thumbnail> thumbnail;
    qreal currentOpacity;
    bool highlighted, hovered, mDrawLabel;
    int thumbnailSize, marginY, marginX, textHeight;
    QRectF highlightRect, nameRect, nameTextRect, labelTextRect;
    QColor highlightColor, nameColor;
    QFont font, fontSmall;
    QFontMetrics *fm, *fmSmall;
    QPropertyAnimation *opacityAnimation;
    const qreal inactiveOpacity = 0.86;
    const int opacityAnimationSpeed = 80;
};

#endif // THUMBNAILWIDGET_H

