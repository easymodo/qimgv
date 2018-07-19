#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel(QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    state(EMPTY),
    highlightStyle(HIGHLIGHT_TOPBAR),
    thumbnail(nullptr),
    highlighted(false),
    hovered(false),
    mDrawLabel(true),
    thumbnailSize(50),
    marginX(1),
    marginY(3)
{
    setAcceptHoverEvents(true);
    nameColor.setRgb(20, 20, 20, 255);
    qreal fntSz = font.pointSizeF();
    if(fntSz > 0) {
        font.setPointSizeF(font.pointSizeF() * 0.9f);
        fontSmall.setPointSizeF(fontSmall.pointSizeF() * 0.85f);
    }
    font.setBold(true);
    fontSmall.setBold(true);
    fm = new QFontMetrics(font);
    fmSmall = new QFontMetrics(fontSmall);

    opacityAnimation = new QPropertyAnimation(this, "currentOpacity");
    opacityAnimation->setEasingCurve(QEasingCurve::InQuad);
    opacityAnimation->setDuration(opacityAnimationSpeed);
    currentOpacity = inactiveOpacity;

    setThumbnailSize(100);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void ThumbnailLabel::readSettings() {
    highlightColor.setRgb(settings->accentColor().rgb());
}

void ThumbnailLabel::setThumbnailSize(int size) {
    if(thumbnailSize != size && size > 0) {
        this->state = EMPTY;
        thumbnail = nullptr;
        thumbnailSize = size;
        updateGeometry();

        highlightBarRect = QRectF(marginX, 0, width() - marginX * 2, marginY);
        nameRect = QRectF(highlightBarRect.left(), highlightBarRect.height(),
                         highlightBarRect.width(), fm->height() * 1.6);
        update();
    }
}

void ThumbnailLabel::setMargins(int x, int y) {
    marginX = x;
    marginY = y;
}

void ThumbnailLabel::setDrawLabel(bool mode) {
    if(mDrawLabel != mode) {
        mDrawLabel = mode;
        update();
    }
}

void ThumbnailLabel::setHightlightStyle(HighlightStyle style) {
    if(highlightStyle != style) {
        highlightStyle = style;
        if(isHighlighted() || isHovered())
            update();
    }
}

void ThumbnailLabel::updateGeometry() {
    QGraphicsWidget::updateGeometry();
}

void ThumbnailLabel::setGeometry(const QRectF &rect) {
    QGraphicsWidget::setGeometry(QRectF(rect.topLeft(), boundingRect().size()));
}

QRectF ThumbnailLabel::geometry() const {
    return QRectF(QGraphicsWidget::geometry().topLeft(), boundingRect().size());
}

QSizeF ThumbnailLabel::effectiveSizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    return sizeHint(which, constraint);
}

void ThumbnailLabel::setThumbnail(std::shared_ptr<Thumbnail> _thumbnail) {
    if(_thumbnail) {
        thumbnail = _thumbnail;
        setupLabel();
        update();
    }
}

void ThumbnailLabel::setupLabel() {
    if(thumbnail && !thumbnail->label().isEmpty()) {
        int heightTextMargin = (nameRect.height() - fm->height()) / 2;
        nameTextRect = nameRect.adjusted(4, heightTextMargin, -4, -heightTextMargin);
        labelTextRect.setWidth(fmSmall->width(thumbnail->label()));
        labelTextRect.setHeight(fmSmall->height());
        labelTextRect.moveCenter(nameRect.center());
        labelTextRect.moveRight(nameTextRect.right());
        nameTextRect.adjust(0, 0, -labelTextRect.width() - 4, 0);
    }
}

void ThumbnailLabel::setHighlighted(bool mode, bool smooth) {
    if(highlighted != mode) {
        highlighted = mode;
        if(highlighted) {
            if(opacityAnimation->state() == QAbstractAnimation::Running)
                opacityAnimation->stop();
            setOpacity(1.0f, smooth);
        } else if(!isHovered()) {
            if(!mode)
                setOpacity(inactiveOpacity, smooth);
        }
    }
}

bool ThumbnailLabel::isHighlighted() {
    return highlighted;
}

// method for propertyAnimation. Use setOpacity() instead
void ThumbnailLabel::propertySetOpacity(qreal amount) {
    if(amount != currentOpacity) {
        currentOpacity = amount;
        update();
    }
}

qreal ThumbnailLabel::propertyOpacity() {
    return currentOpacity;
}

void ThumbnailLabel::setOpacity(qreal amount, bool smooth) {
    if(opacityAnimation->state() == QAbstractAnimation::Running)
        opacityAnimation->stop();
    if(amount != currentOpacity) {
        if(smooth) {
            opacityAnimation->setStartValue(currentOpacity);
            opacityAnimation->setEndValue(amount);
            opacityAnimation->start(QAbstractAnimation::KeepWhenStopped);
        } else {
            currentOpacity = amount;
            update();
        }
    }
}

QRectF ThumbnailLabel::boundingRect() const {
    //qDebug() << "boundingRect: " << geometry();
    //return QRectF(QPointF(0, 0), this->geometry().size());
    return QRectF(0, 0, thumbnailSize + marginX * 2, thumbnailSize + marginY * 2);
}

int ThumbnailLabel::width() {
    return boundingRect().width();
}

int ThumbnailLabel::height() {
    return boundingRect().height();
}

void ThumbnailLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget)
    Q_UNUSED(option)
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setOpacity(1.0f);
    qreal dpr = painter->paintEngine()->paintDevice()->devicePixelRatioF();
    drawHighlight(painter);
    if(!thumbnail) {
        QPixmap* loadingIcon = shrRes->loadingIcon72();
        drawThumbnail(painter, dpr, loadingIcon);
    } else {
        if(thumbnail->pixmap().get()->width() == 0) {
            QPixmap* errorIcon = shrRes->loadingErrorIcon72();
            drawThumbnail(painter, dpr, errorIcon);
        } else {
            painter->setOpacity(currentOpacity);
            drawThumbnail(painter, dpr, thumbnail->pixmap().get());
        }
        if(mDrawLabel) {
            drawLabel(painter);
        }
    }
}

inline
void ThumbnailLabel::drawHighlight(QPainter *painter) {
    if(highlightStyle == HIGHLIGHT_TOPBAR) {
        if(isHighlighted()) {
            painter->fillRect(highlightBarRect, highlightColor);
        } else if (isHovered()) {
            painter->setOpacity(0.6f);
            painter->fillRect(highlightBarRect, highlightColor);
            painter->setOpacity(1.0f);
        }
    } else if(highlightStyle == HIGHLIGHT_BACKGROUND) {
        if(isHighlighted()) {
            //painter->fillRect(boundingRect().adjusted(-marginX,-marginY,marginX,marginY), highlightColor);
            painter->fillRect(boundingRect(), highlightColor);
        }
    }
}

inline
void ThumbnailLabel::drawLabel(QPainter *painter) {
    // text background
    painter->setOpacity(0.95f);
    painter->fillRect(nameRect, nameColor);
    painter->setOpacity(currentOpacity);
    // filename
    painter->setFont(font);
    painter->setPen(QColor(230, 230, 230, 255));
    painter->drawText(nameTextRect, Qt::TextSingleLine, thumbnail->name());
    // additional info
    painter->setFont(fontSmall);
    painter->setPen(QColor(160, 160, 160, 255));
    painter->drawText(labelTextRect, Qt::TextSingleLine, thumbnail->label());
}

inline
void ThumbnailLabel::drawThumbnail(QPainter* painter, qreal dpr, const QPixmap *pixmap) {
    QPointF drawPosCentered(width()  / 2 - pixmap->width()  / (2 * qApp->devicePixelRatio()),
                            height() / 2 - pixmap->height() / (2 * qApp->devicePixelRatio()));
    painter->drawPixmap(drawPosCentered, *pixmap, QRectF(QPoint(0,0), pixmap->size()));
}

inline
void ThumbnailLabel::drawIcon(QPainter* painter, qreal dpr, const QPixmap *pixmap) {

}

QSizeF ThumbnailLabel::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    Q_UNUSED(which);
    Q_UNUSED(constraint);

    return boundingRect().size();
}

void ThumbnailLabel::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    setHovered(true);
}

void ThumbnailLabel::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    setHovered(false);
}

void ThumbnailLabel::setHovered(bool mode) {
    hovered = mode;
    if(!isHighlighted()) {
        if(mode) {
            setOpacity(1.0f, false);
        } else {
            setOpacity(inactiveOpacity, false);
        }
    }
}

bool ThumbnailLabel::isHovered() {
    return hovered;
}

ThumbnailLabel::~ThumbnailLabel() {
    delete opacityAnimation;
    delete fm;
    delete fmSmall;
}
