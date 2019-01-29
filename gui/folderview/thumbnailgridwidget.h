#ifndef THUMBNAILGRIDWIDGET_H
#define THUMBNAILGRIDWIDGET_H

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
    virtual void drawIcon(QPainter *painter, qreal dpr, const QPixmap *pixmap) Q_DECL_OVERRIDE;

    QColor shadowColor;
    void updateThumbnailDrawPosition() Q_DECL_OVERRIDE;
    void drawThumbnail(QPainter *painter, qreal dpr, const QPixmap *pixmap) Q_DECL_OVERRIDE;
private:
    bool nameFits;
    int labelSpacing;
};

#endif // THUMBNAILGRIDWIDGET_H
