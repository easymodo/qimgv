#pragma once

#include <QGraphicsWidget>
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QPaintEngine>
#include <cmath>
#include "sourcecontainers/thumbnail.h"
#include "settings.h"
#include "sharedresources.h"

class ThumbnailWidget : public QGraphicsWidget {
    Q_OBJECT

public:
    ThumbnailWidget(QGraphicsItem *parent = nullptr);
    ~ThumbnailWidget() Q_DECL_OVERRIDE;

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    bool isLoaded;
    void setThumbnail(std::shared_ptr<Thumbnail> _thumbnail);

    void setHighlighted(bool x);
    bool isHighlighted();

    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

    qreal width();
    qreal height();
    void setThumbnailSize(int size);

    void setGeometry(const QRectF &rect) Q_DECL_OVERRIDE;

    virtual QRectF geometry() const;
    QSizeF effectiveSizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void setDrawLabel(bool mode);
    void setPadding(int x, int y);
    int thumbnailSize();
    void reset();
    void unsetThumbnail();

protected slots:
    virtual void readSettings();

protected:
    virtual void setupLayout();
    virtual void drawThumbnail(QPainter* painter, const QPixmap *pixmap);
    virtual void drawIcon(QPainter *painter, const QPixmap *pixmap);
    virtual void drawHighlight(QPainter *painter);
    virtual void drawHover(QPainter *painter);
    virtual void drawLabel(QPainter *painter);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const Q_DECL_OVERRIDE;
    void updateGeometry() Q_DECL_OVERRIDE;
    void setHovered(bool);
    bool isHovered();
    virtual void updateHighlightRect();

    virtual void updateThumbnailDrawPosition();

    std::shared_ptr<Thumbnail> thumbnail;
    bool highlighted, hovered, mDrawLabel, animateHover;
    int mThumbnailSize, paddingX, paddingY, textHeight;
    QRectF highlightRect, nameRect, nameTextRect, labelTextRect;
    QColor highlightColor, nameColor;
    QFont font, fontSmall;
    QFontMetrics *fm, *fmSmall;
    QRect drawRectCentered;
};
