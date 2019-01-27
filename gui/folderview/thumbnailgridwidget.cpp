#include "thumbnailgridwidget.h"

ThumbnailGridWidget::ThumbnailGridWidget(QGraphicsItem* parent)
    : ThumbnailWidget(parent),
      nameFits(true)
{
    outlineColor.setRgb(255,255,255,60);
}

QRectF ThumbnailGridWidget::boundingRect() const {
    return QRectF(0, 0,
                  mThumbnailSize + marginX * 2,
                  mThumbnailSize + marginY * 2 + textHeight * 1.7);
}

void ThumbnailGridWidget::setupLayout() {
    highlightRect = boundingRect();
    highlightRect.setBottom(highlightRect.bottom() - marginY);
    if(thumbnail) {
        highlightRect.setTop(mThumbnailSize - qMax(thumbnail->pixmap()->height(), mThumbnailSize));
    }
    nameRect = QRectF(marginX, marginY + mThumbnailSize,
                      mThumbnailSize, fm->height() * 1.7);
    nameTextRect = nameRect.adjusted(4, 0, -4, 0);
    nameFits = !(thumbnail && fm->width(thumbnail->name()) >= nameTextRect.width());
}

void ThumbnailGridWidget::drawHighlight(QPainter *painter) {
    if(isHighlighted()) {
        painter->fillRect(highlightRect, QColor(77,78,79));
    }
}

void ThumbnailGridWidget::drawHover(QPainter *painter) {
    if(isHovered()) {
        painter->fillRect(highlightRect, QColor(255,255,255, 15));
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
        if(qMax(thumbnail->pixmap()->width(), thumbnail->pixmap()->height()) == floor(mThumbnailSize * dpr)) {
            // correctly sized  thumbnail
            QPoint topLeft(width() / 2.0 - thumbnail->pixmap()->width() / (2.0 * dpr),
                           marginY + mThumbnailSize - thumbnail->pixmap()->height() / dpr);
            drawRectCentered = QRect(topLeft, thumbnail->pixmap()->size() / dpr);
        } else {
            // old size pixmap, scaling
            QSize scaled = thumbnail->pixmap()->size().scaled(mThumbnailSize, mThumbnailSize, Qt::KeepAspectRatio);
            QPoint topLeft((width() - scaled.width()) / 2.0,
                           marginY + mThumbnailSize - scaled.height());
            drawRectCentered = QRect(topLeft, scaled);
        }
    }
}

void ThumbnailGridWidget::drawIcon(QPainter *painter, qreal dpr, const QPixmap *pixmap) {
    QPointF drawPosCentered(width()  / 2 - pixmap->width()  / (2 * dpr),
                            height() / 2 - pixmap->height() / (2 * dpr));
    painter->drawPixmap(drawPosCentered, *pixmap, QRectF(QPoint(0,0), pixmap->size()));
}
