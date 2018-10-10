#include "thumbnailgridwidget.h"

ThumbnailGridWidget::ThumbnailGridWidget(QGraphicsItem* parent)
    : ThumbnailWidget(parent),
      nameFits(true)
{
    outlineColor.setRgb(255,255,255,60);
}

QRectF ThumbnailGridWidget::boundingRect() const {
    return QRectF(0, 0,
                  thumbnailSize + marginX * 2,
                  thumbnailSize + marginY * 2 + textHeight * 1.7);
}

void ThumbnailGridWidget::setupLayout() {
    highlightRect = boundingRect();
    highlightRect.setBottom(highlightRect.bottom() - marginY);
    if(thumbnail) {
        highlightRect.setTop(thumbnailSize - thumbnail->pixmap()->height()/qApp->devicePixelRatio());
    }
    nameRect = QRectF(marginX, marginY + thumbnailSize,
                     thumbnailSize, fm->height() * 1.7);
    nameTextRect = nameRect.adjusted(4, 0, -4, 0);
    if(thumbnail && fm->width(thumbnail->name()) >= nameTextRect.width()) {
        nameFits = false;
    }
}

void ThumbnailGridWidget::drawHighlight(QPainter *painter) {
    if(isHighlighted()) {
        painter->fillRect(highlightRect, QColor(77,78,79));
    }
}

void ThumbnailGridWidget::drawHover(QPainter *painter) {
    if(isHovered()) {
        painter->fillRect(highlightRect, QColor(255,255,255, 10));
    }
}

void ThumbnailGridWidget::drawLabel(QPainter *painter) {
    // filename
    int flags;
    if(nameFits)
        flags = Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignHCenter;
    else
        flags = Qt::TextSingleLine | Qt::AlignVCenter;
    painter->setFont(font);
    painter->setPen(QColor(230, 230, 230, 255));
    painter->drawText(nameTextRect, flags, thumbnail->name());
    // additional info
    //painter->setFont(fontSmall);
    //painter->setPen(QColor(160, 160, 160, 255));
    //painter->drawText(labelTextRect, Qt::TextSingleLine, thumbnail->label());
}

void ThumbnailGridWidget::updateThumbnailDrawPosition() {
    if(thumbnail) {
        qreal dpr = qApp->devicePixelRatio();
        drawPosCentered = QPointF((width() / 2.0 - thumbnail->pixmap()->width() / (2.0 * dpr)),
                                  (marginY + thumbnailSize - thumbnail->pixmap()->height() / dpr));
    }
}

void ThumbnailGridWidget::drawThumbnail(QPainter *painter, qreal dpr, const QPixmap *pixmap) {
    painter->drawPixmap(drawPosCentered, *pixmap);
}

void ThumbnailGridWidget::drawIcon(QPainter *painter, qreal dpr, const QPixmap *pixmap) {
    QPointF drawPosCentered(width()  / 2 - pixmap->width()  / (2 * dpr),
                            height() / 2 - pixmap->height() / (2 * dpr));
    painter->drawPixmap(drawPosCentered, *pixmap, QRectF(QPoint(0,0), pixmap->size()));
}
