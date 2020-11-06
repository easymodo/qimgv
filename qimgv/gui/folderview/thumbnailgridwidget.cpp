#include "thumbnailgridwidget.h"

ThumbnailGridWidget::ThumbnailGridWidget(QGraphicsItem* parent)
    : ThumbnailWidget(parent),
      labelSpacing(9),
      margin(2)
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    font.setBold(false);
    shadowColor.setRgb(0,0,0,60);
    margin = 2;
    readSettings();
}

void ThumbnailGridWidget::updateBoundingRect() {
    if(mDrawLabel)
        mBoundingRect = QRectF(0, 0,
                               mThumbnailSize + (padding + margin) * 2,
                               mThumbnailSize + (padding + margin) * 2 + labelSpacing + textHeight * 2);
    else
        mBoundingRect = QRectF(0, 0,
                               mThumbnailSize + (padding + margin) * 2,
                               mThumbnailSize + (padding + margin) * 2);
}

void ThumbnailGridWidget::setupLayout() {
    if(mDrawLabel) {
        nameRect = QRectF(padding + margin, padding + margin + mThumbnailSize + labelSpacing,
                          mThumbnailSize,
                          fm->height());
        infoRect = nameRect.adjusted(0, fm->height() + 2, 0, fm->height() + 2);
    }
}

void ThumbnailGridWidget::drawHighlight(QPainter *painter) {
    if(isHighlighted())
        painter->fillRect(highlightRect, highlightColor);
}

void ThumbnailGridWidget::drawThumbnail(QPainter *painter, const QPixmap *pixmap) {
    if(!thumbnail->hasAlphaChannel())
        painter->fillRect(drawRectCentered.adjusted(3,3,3,3), shadowColor);
    painter->drawPixmap(drawRectCentered, *pixmap);
    if(isHovered()) {
        auto op = painter->opacity();
        auto mode = painter->compositionMode();
        painter->setCompositionMode(QPainter::CompositionMode_Plus);
        painter->setOpacity(0.2f);
        painter->drawPixmap(drawRectCentered, *thumbnail->pixmap());
        painter->setOpacity(op);
        painter->setCompositionMode(mode);
    }
}

void ThumbnailGridWidget::readSettings() {
    highlightColor = settings->colorScheme().accent;
}

void ThumbnailGridWidget::drawHoverHighlight(QPainter *painter) {
    auto op = painter->opacity();
    painter->fillRect(highlightRect, settings->colorScheme().folderview_hc);
    painter->setOpacity(op);
}

void ThumbnailGridWidget::drawLabel(QPainter *painter) {
    if(thumbnail) {
        drawSingleLineText(painter, nameRect, thumbnail->name(), settings->colorScheme().text_hc2);
        drawSingleLineText(painter, infoRect, thumbnail->label(), settings->colorScheme().text_lc);
    }
}

// todo use shared layer, then merge it down at the end
void ThumbnailGridWidget::drawSingleLineText(QPainter *painter, QRectF rect, QString text, const QColor &color) {
    qreal dpr = qApp->devicePixelRatio();
    bool fits = !(fm->horizontalAdvance(text) >= rect.width());

    // filename
    int flags;
    painter->setFont(font);
    if(fits) {
        flags = Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignHCenter;
        painter->setPen(color);
        painter->drawText(rect, flags, text);
    } else {
        // fancy variant with text fade effect - uses temporary surface to paint; slow
        QPixmap textLayer(rect.width() * dpr, rect.height() * dpr);
        textLayer.fill(Qt::transparent);
        textLayer.setDevicePixelRatio(dpr);
        QPainter textPainter(&textLayer);
        // paint text onto tmp layer
        flags = Qt::TextSingleLine | Qt::AlignVCenter;
        textPainter.setPen(color);
        QRect textRect = QRect(0, 0, rect.width(), rect.height());
        textPainter.drawText(textRect, flags, text);
        QRectF fadeRect = textRect.adjusted(textRect.width() - 6,0,0,0);
        // fade effect
        QLinearGradient gradient(fadeRect.topLeft(), fadeRect.topRight());
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(1, Qt::red); // any color, this is just a transparency mask
        textPainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        textPainter.fillRect(fadeRect, gradient);
        // write text layer into graphicsitem
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter->drawPixmap(rect.topLeft(), textLayer);
    }
}

void ThumbnailGridWidget::updateHighlightRect() {
    if(!mDrawLabel || drawRectCentered.height() >= drawRectCentered.width()) {
        highlightRect = boundingRect().adjusted(margin, margin, -margin, -margin);
    } else {
        highlightRect.setBottom(height() - margin);
        highlightRect.setLeft(margin);
        highlightRect.setRight(width() - margin);
        if(!thumbnail || !thumbnail->pixmap())
            highlightRect.setTop(drawRectCentered.top() - padding);
        else // ensure we get equal padding on the top & sides
            highlightRect.setTop(qMax(drawRectCentered.top() - drawRectCentered.left() + margin, margin));
    }
}

// TODO: simplify
void ThumbnailGridWidget::updateThumbnailDrawPosition() {
    if(thumbnail) {
        qreal dpr = qApp->devicePixelRatio();
        if(isLoaded) {
            QPoint topLeft;
            // correctly sized thumbnail
            if(mDrawLabel) {
                // snap thumbnail to bottom when drawing label
                topLeft.setX(width() / 2.0 - thumbnail->pixmap()->width() / (2.0 * dpr));
                topLeft.setY(padding + margin + mThumbnailSize - thumbnail->pixmap()->height() / dpr);
            } else {
                // center otherwise
                topLeft.setX(width() / 2.0 - thumbnail->pixmap()->width() / (2.0 * dpr));
                topLeft.setY(height() / 2.0 - thumbnail->pixmap()->height() / (2.0 * dpr));
            }
            drawRectCentered = QRect(topLeft, thumbnail->pixmap()->size() / dpr);
        } else {
            // old size pixmap, scaling
            QSize scaled = thumbnail->pixmap()->size().scaled(mThumbnailSize, mThumbnailSize, Qt::KeepAspectRatio);
            QPoint topLeft;
            if(mDrawLabel) {
                // snap thumbnail to bottom when drawing label
                topLeft.setX((width() - scaled.width()) / 2.0);
                topLeft.setY(padding + margin + mThumbnailSize - scaled.height());
            } else {
                // center otherwise
                topLeft.setX((width() - scaled.width()) / 2.0);
                topLeft.setY((height() - scaled.height()) / 2.0);
            }
            drawRectCentered = QRect(topLeft, scaled);
        }
    }
}

void ThumbnailGridWidget::drawIcon(QPainter *painter, const QPixmap *pixmap) {
    QPointF drawPosCentered(width()  / 2 - pixmap->width()  / (2 * pixmap->devicePixelRatioF()),
                            height() / 2 - pixmap->height() / (2 * pixmap->devicePixelRatioF()));
    painter->drawPixmap(drawPosCentered, *pixmap, QRectF(QPoint(0,0), pixmap->size()));
}
