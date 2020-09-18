#include "thumbnailgridwidget.h"

ThumbnailGridWidget::ThumbnailGridWidget(QGraphicsItem* parent)
    : ThumbnailWidget(parent),
      labelSpacing(7)
{
    font.setBold(false);
    shadowColor.setRgb(0,0,0,60);
    readSettings();
}

QRectF ThumbnailGridWidget::boundingRect() const {
    if(mDrawLabel)
        return QRectF(0, 0, mThumbnailSize + paddingX * 2,
                            mThumbnailSize + paddingY * 2 + labelSpacing + textHeight * 2);
    else
        return QRectF(0, 0, mThumbnailSize + paddingX * 2,
                            mThumbnailSize + paddingY * 2);
}

void ThumbnailGridWidget::setupLayout() {
    if(mDrawLabel) {
        nameRect = QRectF(paddingX, paddingY + mThumbnailSize + labelSpacing,
                          mThumbnailSize, fm->height());
        infoRect = nameRect.adjusted(0,fm->height() + 2,0,fm->height() + 2);
    }
}

void ThumbnailGridWidget::drawHighlight(QPainter *painter) {
    if(isHighlighted()) {
        QPainterPath path;
        // fill
        path.addRoundedRect(highlightRect, 3, 3);
        painter->fillPath(path, highlightColor);
        // outline
        path.clear();
        QRectF adj = static_cast<QRectF>(highlightRect).adjusted(0.5f, 0.5f, -0.5f, -0.5f);
        path.addRoundedRect(adj, 3, 3);
        auto op = painter->opacity();
        painter->setOpacity(0.03f);
        painter->setPen(Qt::white);
        painter->drawPath(path);
        painter->setOpacity(op);
    }
}

void ThumbnailGridWidget::drawThumbnail(QPainter *painter, const QPixmap *pixmap) {
    qreal dpr = qApp->devicePixelRatio();
    /*if(!thumbnail->hasAlphaChannel()) {
        // rounded corners variant - via tmp pixmap layer
        // slower but FANCY
        QPixmap surface(width() * dpr, height() * dpr);
        surface.fill(Qt::transparent);
        surface.setDevicePixelRatio(dpr);
        QPainter spainter(&surface);
        // rounded mask
        QPainterPath path;
        path.addRoundedRect(drawRectCentered, 3, 3);
        spainter.fillPath(path, Qt::red);
        // rounded thumbnail
        spainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        spainter.drawPixmap(drawRectCentered, *pixmap);
        spainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        // outline
        path.clear();
        QRectF adj = static_cast<QRectF>(drawRectCentered).adjusted(0.5f, 0.5f, -0.5f, -0.5f);
        path.addRoundedRect(adj, 3, 3);
        spainter.setOpacity(0.03f);
        spainter.setPen(Qt::white);
        spainter.drawPath(path);
        // drop shadow (todo - maybe soft shadows?)
        path.clear();
        path.addRoundedRect(drawRectCentered.adjusted(2,2,2,2),3,3); // offset=3 may look better.. make this configurable?
        painter->fillPath(path, shadowColor);
        // write thumbnail layer into graphicsitem
        painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter->drawPixmap(QPointF(0,0), surface);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    } else {
        painter->drawPixmap(drawRectCentered, *pixmap);
    }
    */
    // draw a shadow rectangle
    if(!thumbnail->hasAlphaChannel())
        painter->fillRect(drawRectCentered.adjusted(3,3,3,3), shadowColor);
    painter->drawPixmap(drawRectCentered, *pixmap);
    if(isHovered()) {
    //    painter->fillRect(drawRectCentered, QColor(255,255,255, 18));
    }
}

void ThumbnailGridWidget::readSettings() {
    highlightColor = settings->colorScheme().accent;
}

void ThumbnailGridWidget::drawHover(QPainter *painter) {
    //Q_UNUSED(painter)
    QPainterPath path;
    path.addRoundedRect(highlightRect, 3, 3);
    auto op = painter->opacity();
    painter->setOpacity(0.55f);
    painter->fillPath(path, settings->colorScheme().accent);
    painter->setOpacity(op);
}

void ThumbnailGridWidget::drawLabel(QPainter *painter) {
    if(thumbnail) {
        drawSingleLineText(painter, nameRect, thumbnail->name(), settings->colorScheme().text_hc2);
        drawSingleLineText(painter, infoRect, thumbnail->label(), settings->colorScheme().text_lc1);
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
        painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter->drawPixmap(rect.topLeft(), textLayer);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
}

void ThumbnailGridWidget::updateHighlightRect() {
    if(!mDrawLabel || drawRectCentered.height() >= drawRectCentered.width()) {
        highlightRect = boundingRect();
    } else {
        highlightRect = drawRectCentered.adjusted(-paddingX, -paddingY, paddingX, paddingY);
        highlightRect.setBottom(height());
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
                topLeft.setY(paddingY + mThumbnailSize - thumbnail->pixmap()->height() / dpr);
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
                topLeft.setY(paddingY + mThumbnailSize - scaled.height());
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
