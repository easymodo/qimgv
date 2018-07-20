#include "thumbnailwidget.h"

ThumbnailWidget::ThumbnailWidget(QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    state(EMPTY),
    thumbnail(nullptr),
    highlighted(false),
    hovered(false),
    mDrawLabel(true),
    thumbnailSize(100),
    marginX(1),
    marginY(3),
    textHeight(5)
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
    textHeight = fm->height();

    opacityAnimation = new QPropertyAnimation(this, "currentOpacity");
    opacityAnimation->setEasingCurve(QEasingCurve::InQuad);
    opacityAnimation->setDuration(opacityAnimationSpeed);
    currentOpacity = inactiveOpacity;

    setThumbnailSize(100);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void ThumbnailWidget::readSettings() {
    highlightColor.setRgb(settings->accentColor().rgb());
}

void ThumbnailWidget::setThumbnailSize(int size) {
    if(thumbnailSize != size && size > 0) {
        this->state = EMPTY;
        thumbnail = nullptr;
        thumbnailSize = size;
        updateGeometry();
        setupLayout();
        update();
    }
}

void ThumbnailWidget::setMargins(int x, int y) {
    marginX = x;
    marginY = y;
}

void ThumbnailWidget::setDrawLabel(bool mode) {
    if(mDrawLabel != mode) {
        mDrawLabel = mode;
        update();
    }
}

void ThumbnailWidget::updateGeometry() {
    QGraphicsWidget::updateGeometry();
}

void ThumbnailWidget::setGeometry(const QRectF &rect) {
    QGraphicsWidget::setGeometry(QRectF(rect.topLeft(), boundingRect().size()));
}

QRectF ThumbnailWidget::geometry() const {
    return QRectF(QGraphicsWidget::geometry().topLeft(), boundingRect().size());
}

QSizeF ThumbnailWidget::effectiveSizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    return sizeHint(which, constraint);
}

void ThumbnailWidget::setThumbnail(std::shared_ptr<Thumbnail> _thumbnail) {
    if(_thumbnail) {
        thumbnail = _thumbnail;
        setupLayout();
        update();
    }
}

void ThumbnailWidget::setupLayout() {
    highlightRect = QRectF(marginX, 0, width() - marginX * 2, marginY);
    nameRect = QRectF(highlightRect.left(), highlightRect.height(),
                     highlightRect.width(), textHeight * 1.6);

    if(thumbnail && !thumbnail->label().isEmpty()) {
        int heightTextMargin = (nameRect.height() - textHeight) / 2;
        nameTextRect = nameRect.adjusted(4, heightTextMargin, -4, -heightTextMargin);
        labelTextRect.setWidth(fmSmall->width(thumbnail->label()));
        labelTextRect.setHeight(fmSmall->height());
        labelTextRect.moveCenter(nameRect.center());
        labelTextRect.moveRight(nameTextRect.right());
        nameTextRect.adjust(0, 0, -labelTextRect.width() - 4, 0);
    }
}

void ThumbnailWidget::setHighlighted(bool mode, bool smooth) {
    if(highlighted != mode) {
        highlighted = mode;
        if(highlighted) {
            if(opacityAnimation->state() == QAbstractAnimation::Running)
                opacityAnimation->stop();
            setOpacity(1.0f, smooth);
        } else {
            if(!mode)
                setOpacity(inactiveOpacity, smooth);
        }
    }
}

bool ThumbnailWidget::isHighlighted() {
    return highlighted;
}

// method for propertyAnimation. Use setOpacity() instead
void ThumbnailWidget::propertySetOpacity(qreal amount) {
    if(amount != currentOpacity) {
        currentOpacity = amount;
        update();
    }
}

qreal ThumbnailWidget::propertyOpacity() {
    return currentOpacity;
}

void ThumbnailWidget::setOpacity(qreal amount, bool smooth) {
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

QRectF ThumbnailWidget::boundingRect() const {
    //qDebug() << "boundingRect: " << geometry();
    //return QRectF(QPointF(0, 0), this->geometry().size());
    return QRectF(0, 0, thumbnailSize + marginX * 2, thumbnailSize + marginY * 2);
}

int ThumbnailWidget::width() {
    return boundingRect().width();
}

int ThumbnailWidget::height() {
    return boundingRect().height();
}

void ThumbnailWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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

void ThumbnailWidget::drawHighlight(QPainter *painter) {
    if(isHighlighted()) {
        painter->fillRect(highlightRect, highlightColor);
    } else if (isHovered()) {
        painter->setOpacity(0.6f);
        painter->fillRect(highlightRect, highlightColor);
        painter->setOpacity(1.0f);
    }
}

void ThumbnailWidget::drawLabel(QPainter *painter) {
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

void ThumbnailWidget::drawThumbnail(QPainter* painter, qreal dpr, const QPixmap *pixmap) {
    QPointF drawPosCentered(width()  / 2 - pixmap->width()  / (2 * qApp->devicePixelRatio()),
                            height() / 2 - pixmap->height() / (2 * qApp->devicePixelRatio()));

    //QPointF drawPosCentered(width()/2 - pixmap->width()/(2*qApp->devicePixelRatio()),
    //                        highlightBarHeight + (thumbnailSize)/2 - pixmap->height()/(2*qApp->devicePixelRatio()));

    painter->drawPixmap(drawPosCentered, *pixmap, QRectF(QPoint(0,0), pixmap->size()));
}

void ThumbnailWidget::drawIcon(QPainter* painter, qreal dpr, const QPixmap *pixmap) {

}

QSizeF ThumbnailWidget::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    Q_UNUSED(which);
    Q_UNUSED(constraint);

    return boundingRect().size();
}

void ThumbnailWidget::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    setHovered(true);
}

void ThumbnailWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    setHovered(false);
}

void ThumbnailWidget::setHovered(bool mode) {
    hovered = mode;
    if(!isHighlighted()) {
        if(mode) {
            setOpacity(1.0f, false);
        } else {
            setOpacity(inactiveOpacity, false);
        }
    }
}

bool ThumbnailWidget::isHovered() {
    return hovered;
}

ThumbnailWidget::~ThumbnailWidget() {
    delete opacityAnimation;
    delete fm;
    delete fmSmall;
}
