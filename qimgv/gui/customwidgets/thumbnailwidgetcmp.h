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
#include "utils/imagelib.h"
#include "settings.h"
#include "sharedresources.h"

enum ThumbnailStyle {
    THUMB_SIMPLE,
    THUMB_NORMAL,
    THUMB_NORMAL_CENTERED,
    THUMB_COMPACT
};

class ThumbnailWidgetCmp : public QGraphicsWidget {
    Q_OBJECT

public:
    ThumbnailWidgetCmp(QGraphicsItem *parent = nullptr);
    ~ThumbnailWidgetCmp() override;

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    bool isLoaded;
    void setThumbnail(std::shared_ptr<Thumbnail> _thumbnail);

    void setHighlighted(bool mode);
    bool isHighlighted();

    void setDropHovered(bool mode);
    bool isDropHovered();

    virtual QRectF boundingRect() const override;

    qreal width();
    qreal height();
    void setThumbnailSize(int size);

    void setGeometry(const QRectF &rect) override;

    virtual QRectF geometry() const;
    QSizeF effectiveSizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void setThumbStyle(ThumbnailStyle _style);
    void setPadding(int _padding);
    void setMargins(int _marginX, int _marginY);
    int thumbnailSize();
    void reset();
    void unsetThumbnail();

protected:
    void setupTextLayout();
    void drawThumbnail(QPainter* painter, const QPixmap *pixmap);
    void drawIcon(QPainter *painter, const QPixmap *pixmap);
    void drawHighlight(QPainter *painter);
    void drawHoverBg(QPainter *painter);
    void drawHoverHighlight(QPainter *painter);
    void drawLabel(QPainter *painter);
    void drawDropHover(QPainter *painter);
    void drawSingleLineText(QPainter *painter, QFont &_fnt, QRect rect, QString text, const QColor &color, bool center = true);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;
    void updateGeometry() override;
    void setHovered(bool);
    bool isHovered();
    void updateBackgroundRect();
    void updateThumbnailDrawPosition();

    std::shared_ptr<Thumbnail> thumbnail;
    bool highlighted, hovered, dropHovered;
    int mThumbnailSize, padding, marginX, marginY, labelSpacing, textHeight;
    QRectF bgRect, mBoundingRect;
    QColor shadowColor;
    QFont fontName, fontInfo;
    QRect drawRectCentered, nameRect, infoRect;
    void updateBoundingRect();
    ThumbnailStyle thumbStyle;
};
