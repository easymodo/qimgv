#ifndef THUMBNAILGRIDWIDGET_H
#define THUMBNAILGRIDWIDGET_H

#include "gui/customwidgets/thumbnailwidget.h"

class ThumbnailGridWidget : public ThumbnailWidget {
public:
    ThumbnailGridWidget(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const Q_DECL_OVERRIDE;

protected:
    virtual void setupLayout();
    virtual void drawHighlight(QPainter *painter);
    virtual void drawLabel(QPainter *painter) Q_DECL_OVERRIDE;
    virtual void drawThumbnail(QPainter *painter, qreal dpr, const QPixmap *pixmap) Q_DECL_OVERRIDE;

private:

    bool nameFits;

};

#endif // THUMBNAILGRIDWIDGET_H
