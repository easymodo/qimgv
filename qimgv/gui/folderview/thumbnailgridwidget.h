#pragma once

#include "gui/customwidgets/thumbnailwidget.h"

class ThumbnailGridWidget : public ThumbnailWidget {
public:
    ThumbnailGridWidget(QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const Q_DECL_OVERRIDE;

protected:
    virtual void setupLayout() Q_DECL_OVERRIDE;
    virtual void drawHighlight(QPainter *painter) Q_DECL_OVERRIDE;
    virtual void drawHover(QPainter *painter) Q_DECL_OVERRIDE;
    virtual void drawLabel(QPainter *painter) Q_DECL_OVERRIDE;
    virtual void drawIcon(QPainter *painter, const QPixmap *pixmap) Q_DECL_OVERRIDE;

    QColor shadowColor;
    void updateThumbnailDrawPosition() Q_DECL_OVERRIDE;
    void drawThumbnail(QPainter *painter, const QPixmap *pixmap) Q_DECL_OVERRIDE;
    virtual void readSettings() Q_DECL_OVERRIDE;
    virtual void updateHighlightRect() Q_DECL_OVERRIDE;
    void drawSingleLineText(QPainter *painter, QRectF rect, QString text, const QColor &color);

private:
    int labelSpacing;
    QRectF infoRect;
};
