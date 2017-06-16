#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel() :
    state(EMPTY),
    labelNumber(0),
    showLabel(false),
    thumbnail(NULL),
    highlighted(false),
    hovered(false),
    thumbnailSize(100),
    highlightBarHeight(3),
    marginX(1)
{
    loadingIcon = new QPixmap(":/res/icons/loading72.png");
    outlineColor.setRgb(0,0,0,0);
    nameColor.setRgb(20, 20, 20, 255);

    font.setPixelSize(11);
    font.setBold(true);
    fontSmall.setPixelSize(10);
    fontSmall.setBold(true);
    fm = new QFontMetrics(font);
    fmSmall = new QFontMetrics(fontSmall);

    opacityAnimation = new QPropertyAnimation(this, "currentOpacity");
    opacityAnimation->setEasingCurve(QEasingCurve::InQuad);
    opacityAnimation->setDuration(opacityAnimationSpeed);
    currentOpacity = inactiveOpacity;

    setAcceptHoverEvents(true);
    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void ThumbnailLabel::readSettings() {
    highlightColor.setRgb(settings->accentColor().rgb());
    hoverHighlightColor.setRed(highlightColor.red()/2);
    hoverHighlightColor.setGreen(highlightColor.green()/2);
    hoverHighlightColor.setBlue(highlightColor.blue()/2);
    labelColor.setRgb(settings->accentColor().rgb());
    update();
}

void ThumbnailLabel::setThumbnailSize(int size) {
    if(thumbnailSize != size && size > 0) {
        this->state = EMPTY;
        //delete the old thumbnail
        if(thumbnail) {
            delete thumbnail;
            thumbnail = NULL;
        }
        thumbnailSize = size;
        highlightBarRect = QRect(marginX, 0, width() - marginX * 2, highlightBarHeight);
        nameRect = QRect(highlightBarRect.left(), highlightBarRect.height(),
                         highlightBarRect.width(), 22);
        int heightTextMargin = (nameRect.height() - fm->height()) / 2;
        nameTextRect = nameRect.adjusted(5, heightTextMargin, -5, -heightTextMargin);
        labelRect.setTopLeft(QPointF(width() - 26, nameRect.top()));
        labelRect.setBottomRight(nameRect.bottomRight());
        update();
    }
}

void ThumbnailLabel::setThumbnail(Thumbnail *_thumbnail) {
    if(_thumbnail) {
        thumbnail = _thumbnail;
        //showLabel = settings->showThumbnailLabels() && !thumbnail->label.isEmpty();
        updateLabelWidth();
        update();
    }
}

void ThumbnailLabel::updateLabelWidth() {
    if(showLabel && thumbnail) {
        int labelWidth = fmSmall->width(thumbnail->label);
        labelRect.setWidth(labelWidth + 8);
        labelRect.moveRight(nameRect.right());
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

QRectF ThumbnailLabel::boundingRect() const
{
    return QRectF(0, 0, thumbnailSize + marginX * 2, thumbnailSize + highlightBarHeight);
}

int ThumbnailLabel::width() {
    return boundingRect().width();
}

int ThumbnailLabel::height() {
    return boundingRect().height();
}

int ThumbnailLabel::getThumbnailSize() {
    return thumbnailSize;
}

void ThumbnailLabel::setLabelNum(int num) {
    if(num >= 0)
        labelNumber = num;
    else
        qDebug() << "ThumbnailLabel::setLabelNum - invalid argument.";
}

int ThumbnailLabel::labelNum() {
    return labelNumber;
}

void ThumbnailLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget)

    painter->setOpacity(1.0f);
    if(isHighlighted()) {
        painter->fillRect(highlightBarRect, highlightColor);
    } else if (isHovered()) {
        painter->fillRect(highlightBarRect, hoverHighlightColor);
    }

    if(!thumbnail) {
        painter->drawPixmap((width() - loadingIcon->width()) / 2,
                            (height() - loadingIcon->height() - highlightBarHeight) / 2 + highlightBarHeight,
                            *loadingIcon);
    } else {
        painter->setOpacity(currentOpacity);
        painter->drawPixmap((width() - thumbnail->image->width()) / 2,
                            (height() - thumbnail->image->height() - highlightBarHeight) / 2 + highlightBarHeight,
                            *thumbnail->image);
        //nameLabel
        painter->setOpacity(0.95f);
        painter->fillRect(nameRect, nameColor);
        //setup font
        painter->setFont(font);
        painter->setOpacity(currentOpacity);
        painter->setPen(QColor(245, 245, 245, 255));
        painter->drawText(nameTextRect, Qt::TextSingleLine, thumbnail->name);
        // label with additional info
        if(showLabel) {
            painter->setFont(fontSmall);
            QPointF labelTextPos = labelRect.bottomLeft() + QPointF(3, -6);
            painter->setPen(QColor(160, 160, 160, 255));
            painter->drawText(labelTextPos, thumbnail->label);
        }
    }
}

QSizeF ThumbnailLabel::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    switch(which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
            return QSize(thumbnailSize + marginX * 2, thumbnailSize + highlightBarHeight);
        case Qt::MaximumSize:
            return QSize(thumbnailSize + marginX * 2, thumbnailSize + highlightBarHeight);
        default:
            break;
    }
    return constraint;
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
    delete loadingIcon;
    delete opacityAnimation;
    delete thumbnail;
    delete fm;
    delete fmSmall;
}
