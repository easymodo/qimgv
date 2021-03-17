#pragma once

#include "gui/customwidgets/thumbnailwidget.h"

class ThumbnailGridWidget : public ThumbnailWidget {
public:
    ThumbnailGridWidget(QGraphicsItem *parent = nullptr);

protected:
    virtual void setupLayout() override;
    virtual void drawHighlight(QPainter *painter) override;
    virtual void drawHoverBg(QPainter *painter) override;
    virtual void drawLabel(QPainter *painter) override;
    virtual void drawIcon(QPainter *painter, const QPixmap *pixmap) override;

    QColor shadowColor;
    void updateThumbnailDrawPosition() override;
    void drawThumbnail(QPainter *painter, const QPixmap *pixmap) override;
    virtual void readSettings() override;
    virtual void updateHighlightRect() override;
    void drawSingleLineText(QPainter *painter, QRectF rect, QString text, const QColor &color);

    virtual void updateBoundingRect() override;
private:
    int labelSpacing, margin;
    QRectF infoRect;
    QPixmap *hoverPixmap = nullptr;
};
