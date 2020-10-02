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
    ~ThumbnailWidget() override;

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    bool isLoaded;
    void setThumbnail(std::shared_ptr<Thumbnail> _thumbnail);

    void setHighlighted(bool x);
    bool isHighlighted();

    virtual QRectF boundingRect() const override;

    qreal width();
    qreal height();
    void setThumbnailSize(int size);

    void setGeometry(const QRectF &rect) override;

    virtual QRectF geometry() const;
    QSizeF effectiveSizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void setDrawLabel(bool mode);
    void setPadding(int _padding);
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
    virtual void drawHoverHighlight(QPainter *painter);
    virtual void drawLabel(QPainter *painter);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;
    void updateGeometry() override;
    void setHovered(bool);
    bool isHovered();
    virtual void updateHighlightRect();

    virtual void updateThumbnailDrawPosition();

    std::shared_ptr<Thumbnail> thumbnail;
    bool highlighted, hovered, mDrawLabel, animateHover;
    int mThumbnailSize, padding, textHeight;
    QRectF highlightRect, nameRect, labelTextRect;
    QColor highlightColor, nameColor;
    QFont font, fontSmall;
    QFontMetrics *fm, *fmSmall;
    QRect drawRectCentered;
};
