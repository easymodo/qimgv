#include "thumbnailgridwidget.h"

ThumbnailGridWidget::ThumbnailGridWidget(QGraphicsItem* parent)
    : ThumbnailWidget(parent),
      nameFits(true),
      labelSpacing(7)
{
    shadowColor.setRgb(10,10,10,100);
}

QRectF ThumbnailGridWidget::boundingRect() const {
    if(mDrawLabel)
        return QRectF(0, 0, mThumbnailSize + paddingX * 2,
                            mThumbnailSize + paddingY * 2 + labelSpacing + textHeight);
    else
        return QRectF(0, 0, mThumbnailSize + paddingX * 2,
                            mThumbnailSize + paddingY * 2);
}

void ThumbnailGridWidget::setupLayout() {
    highlightRect = boundingRect();
    if(mDrawLabel) {
        nameRect = QRectF(paddingX, paddingY + mThumbnailSize + labelSpacing,
                          mThumbnailSize, fm->height());
        nameTextRect = nameRect.adjusted(4, 0, -4, 0);
        nameFits = !(thumbnail && fm->width(thumbnail->name()) >= nameTextRect.width());
    }
}

void ThumbnailGridWidget::drawHighlight(QPainter *painter) {
    if(isHighlighted()) {
        painter->fillRect(highlightRect, QColor(77,78,79));
    }
}

void ThumbnailGridWidget::drawThumbnail(QPainter *painter, qreal dpr, const QPixmap *pixmap) {
    painter->fillRect(drawRectCentered.adjusted(3,3,3,3), shadowColor);
    painter->drawPixmap(drawRectCentered, *pixmap);
    if(isHovered()) {
        painter->fillRect(drawRectCentered, QColor(255,255,255, 18));
    }
}

void ThumbnailGridWidget::drawHover(QPainter *painter) {
}

void ThumbnailGridWidget::drawLabel(QPainter *painter) {
    // filename
    int flags;
    if(nameFits)
        flags = Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignHCenter;
    else
        flags = Qt::TextSingleLine | Qt::AlignVCenter;
    painter->setFont(font);
    //shadow
    painter->setPen(shadowColor);
    painter->drawText(nameTextRect.adjusted(2,2,2,2), flags, thumbnail->name());
    //text
    if(isHovered())
        painter->setPen(QColor(242, 242, 242, 255));
    else
        painter->setPen(QColor(215, 215, 215, 255));
    painter->drawText(nameTextRect, flags, thumbnail->name());
    // additional info
    //painter->setFont(fontSmall);
    //painter->setPen(QColor(160, 160, 160, 255));
    //painter->drawText(labelTextRect, Qt::TextSingleLine, thumbnail->label());
}

// TODO: simplify
void ThumbnailGridWidget::updateThumbnailDrawPosition() {
    if(thumbnail) {
        qreal dpr = qApp->devicePixelRatio();
        if(state == LOADED) {
            QPoint topLeft;
            // correctly sized thumbnail
            if(mDrawLabel) {
                // snap thumbnail to bottom when drawing label
                topLeft.setX(width() / 2.0 - thumbnail->pixmap()->width() / (2.0 * dpr));
                topLeft.setY(paddingY + mThumbnailSize - thumbnail->pixmap()->height() / dpr);
            } else {
                // center otherwise
                topLeft.setX(width() / 2.0 - thumbnail->pixmap()->width() / (2.0 * dpr));
                topLeft.setY(height() / 2.0 - thumbnail->pixmap()->height() / (2.0 * dpr));
            }
            // shift by 1px to offset the drop shadow
            drawRectCentered = QRect(topLeft - QPoint(1,1), thumbnail->pixmap()->size() / dpr);
        } else {
            // old size pixmap, scaling
            QSize scaled = thumbnail->pixmap()->size().scaled(mThumbnailSize, mThumbnailSize, Qt::KeepAspectRatio);
            QPoint topLeft;
            if(mDrawLabel) {
                // snap thumbnail to bottom when drawing label
                topLeft.setX((width() - scaled.width()) / 2.0);
                topLeft.setY(paddingY + mThumbnailSize - scaled.height());
            } else {
                // center otherwise
                topLeft.setX((width() - scaled.width()) / 2.0);
                topLeft.setY((height() - scaled.height()) / 2.0);
            }
            drawRectCentered = QRect(topLeft - QPoint(1,1), scaled);
        }
    }
}

void ThumbnailGridWidget::drawIcon(QPainter *painter, qreal dpr, const QPixmap *pixmap) {
    QPointF drawPosCentered(width()  / 2 - pixmap->width()  / (2 * dpr),
                            height() / 2 - pixmap->height() / (2 * dpr));
    painter->drawPixmap(drawPosCentered, *pixmap, QRectF(QPoint(0,0), pixmap->size()));
}
