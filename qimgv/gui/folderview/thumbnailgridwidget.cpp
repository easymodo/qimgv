#include "thumbnailgridwidget.h"

ThumbnailGridWidget::ThumbnailGridWidget(QGraphicsItem* parent)
    : ThumbnailWidget(parent),
      nameFits(true),
      labelSpacing(7)
{
    font.setBold(false);
    shadowColor.setRgb(0,0,0,60);
    readSettings();
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
    if(mDrawLabel) {
        nameRect = QRectF(paddingX, paddingY + mThumbnailSize + labelSpacing,
                          mThumbnailSize, fm->height());
        nameTextRect = nameRect.adjusted(4, 0, -4, 0);
        nameFits = !(thumbnail && fm->horizontalAdvance(thumbnail->name()) >= nameTextRect.width());
    }
}

void ThumbnailGridWidget::drawHighlight(QPainter *painter) {
    if(isHighlighted()) {
        QPainterPath path;
        path.addRoundedRect(highlightRect, 3, 3);
        painter->fillPath(path, highlightColor);
    }
}

void ThumbnailGridWidget::drawThumbnail(QPainter *painter, const QPixmap *pixmap) {
    qreal dpr = qApp->devicePixelRatio();
    if(!thumbnail->hasAlphaChannel()) {
        // square - old variant - fast but looks meh
        /*
        // paint shadow
        painter->fillRect(drawRectCentered.adjusted(3,3,3,3), shadowColor);
        // paint image
        painter->drawPixmap(drawRectCentered, *pixmap);
        // paint outline
        auto op = painter->opacity();
        painter->setOpacity(0.05f);
        painter->setPen(Qt::white);
        QRectF adj = static_cast<QRectF>(drawRectCentered).adjusted(0.5f, 0.5f, -0.5f, -0.5f);
        painter->drawRect(adj);
        painter->setOpacity(op);
        */

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
        spainter.setOpacity(0.05f);
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
    // TODO: OPTIMIZE THIS
    // share secondary layer with drawThumbnail() ?
    // pre-calculate more rects?
    qreal dpr = qApp->devicePixelRatio();
    // filename
    int flags;
    painter->setFont(font);
    if(nameFits) {
        flags = Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignHCenter;
        painter->setPen(settings->colorScheme().text_hc2);
        painter->drawText(nameTextRect, flags, thumbnail->name());
    } else {
        QPixmap textLayer(nameTextRect.width() * dpr, nameTextRect.height() * dpr);
        textLayer.fill(Qt::transparent);
        textLayer.setDevicePixelRatio(dpr);
        QPainter textPainter(&textLayer);
        // paint text onto tmp layer
        flags = Qt::TextSingleLine | Qt::AlignVCenter;
        textPainter.setPen(settings->colorScheme().text_hc2);
        QRect textRect = QRect(0, 0, nameTextRect.width(), nameTextRect.height());
        textPainter.drawText(textRect, flags, thumbnail->name());
        QRectF fadeRect = textRect.adjusted(textRect.width() - 6,0,0,0);
        // fade effect
        QLinearGradient gradient(fadeRect.topLeft(), fadeRect.topRight());
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(1, Qt::red); // any color, this is just a transparency mask
        textPainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        textPainter.fillRect(fadeRect, gradient);
        // write text layer into graphicsitem
        painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter->drawPixmap(nameTextRect.topLeft(), textLayer);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
}

void ThumbnailGridWidget::updateHighlightRect() {
    if(!mDrawLabel || drawRectCentered.height() >= drawRectCentered.width()) {
        highlightRect = boundingRect();
    } else {
        highlightRect = drawRectCentered.adjusted(-paddingX,
                                                  -paddingY,
                                                  paddingX,
                                                  paddingY);
        highlightRect.setBottom(nameTextRect.bottom() + paddingY);
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
