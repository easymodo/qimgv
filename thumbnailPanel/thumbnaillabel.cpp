#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel() :
    state(EMPTY),
    labelNumber(0),
    loaded(false),
    showLabel(true),
    showName(true),
    drawSelectionBorder(false),
    thumbnail(NULL),
    highlighted(false),
    hovered(false),
    borderSize(1),
    thumbnailSize(30),
    currentOpacity(1.0f)
{
    loadingIcon = new QPixmap(":/images/res/icons/loading72.png");
    highlightColor = new QColor();
    hoverHighlightColor = new QColor();
    outlineColor = new QColor(Qt::black);
    nameColor = new QColor(20, 20, 20, 255);
    labelColor = new QColor();

    font.setPixelSize(11);
    font.setBold(true);
    fontsmall.setPixelSize(10);
    fontsmall.setBold(true);
    fm = new QFontMetrics(font);
    fmsmall = new QFontMetrics(fontsmall);

    opacityAnimation = new QPropertyAnimation(this, "currentOpacity");
    opacityAnimation->setEasingCurve(QEasingCurve::InQuad);

    setAcceptHoverEvents(true);
    readSettings();
    setThumbnailSize(100);
}

// call manually from thumbnailStrip
void ThumbnailLabel::readSettings() {
    //thumbnailSize = settings->thumbnailSize();
    drawSelectionBorder = settings->drawThumbnailSelectionBorder();

    highlightColor->setRgb(settings->accentColor().rgb());
    hoverHighlightColor->setRed(highlightColor->red()/2);
    hoverHighlightColor->setGreen(highlightColor->green()/2);
    hoverHighlightColor->setBlue(highlightColor->blue()/2);
    labelColor->setRgb(settings->accentColor().rgb());
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
        highlightRect = boundingRect().marginsRemoved(QMargins(borderSize-1,borderSize-1,borderSize,borderSize));
        nameRect.setTopLeft(QPointF(borderSize, borderSize));
        nameRect.setBottomRight(QPointF(borderSize + thumbnailSize,
                                        borderSize + 22));
        nameRect.setWidth(thumbnailSize);
        labelRect = QRectF(QPointF(borderSize + thumbnailSize - 25,
                                   borderSize),
                           QPointF(borderSize + thumbnailSize,
                                   borderSize + nameRect.height()));
        update();
    }
}

void ThumbnailLabel::setThumbnail(Thumbnail *_thumbnail) {
    if(_thumbnail) {
        thumbnail = _thumbnail;
        loaded = true;
        showLabel = settings->showThumbnailLabels() && !thumbnail->label.isEmpty();
        updateLabelWidth();
        float widthFactor = fm->width(thumbnail->name) / (float)(thumbnailSize - labelRect.width() - 10);
        if(widthFactor > 1) {
            thumbnail->name.truncate(thumbnail->name.length() / widthFactor);
        }
        update();
    }
}

void ThumbnailLabel::updateLabelWidth() {
    if(showLabel && thumbnail) {
        int labelWidth = fmsmall->width(thumbnail->label);
        labelRect.setWidth(labelWidth + 8);
        labelRect.moveRight(nameRect.right());
    }
}

void ThumbnailLabel::setHighlighted(bool x) {
    bool toRepaint = (highlighted == x);
    highlighted = x;
    if(toRepaint) {
        this->update();
    }
}

bool ThumbnailLabel::isHighlighted() {
    return highlighted;
}

bool ThumbnailLabel::isHovered() {
    return hovered;
}

void ThumbnailLabel::setOpacity(qreal amount) {
    if(amount != currentOpacity) {
        currentOpacity = amount;
        update();
    }
}

qreal ThumbnailLabel::opacity() {
    return currentOpacity;
}

void ThumbnailLabel::setOpacityAnimated(qreal amount, int speed) {
    if(amount != opacity()) {
        opacityAnimation->setDuration(speed);
        opacityAnimation->setStartValue(this->opacity());
        opacityAnimation->setEndValue(amount);
        opacityAnimation->start(QAbstractAnimation::KeepWhenStopped);
    }
}

QRectF ThumbnailLabel::boundingRect() const
{
    return QRectF(0, 0, thumbnailSize + borderSize * 2, thumbnailSize + borderSize * 2);
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
    // nice border around the image
    if(isHighlighted()) {
        painter->setPen(QColor(245, 245, 245, 255));
        painter->drawRect(highlightRect);
    } else if(isHovered()) {
        painter->setPen(QColor(180, 180, 180, 255));
        painter->drawRect(highlightRect);
    } else { // inactive outline
        painter->setPen(QColor(110, 110, 110, 255));
        painter->drawRect(highlightRect);
    }

    if(!thumbnail) {
        painter->setOpacity(1.0f);
        painter->drawPixmap((width() - loadingIcon->width()) / 2,
                            borderSize + (height() - loadingIcon->height() - borderSize) / 2,
                            *loadingIcon);
    } else {
        painter->setOpacity(currentOpacity);
        painter->drawPixmap((width() - thumbnail->image->width()) / 2,
                            (height() - thumbnail->image->height()) / 2,
                            *thumbnail->image);

        // inner dark border for contrast
        painter->setPen(QColor(0, 0, 0, 160));
        painter->drawRect(highlightRect.adjusted(1,1,-1,-1));

        painter->setOpacity(0.95f);
        //setup font
        painter->setFont(font);
        //nameLabel
        painter->fillRect(nameRect, *nameColor);
        painter->setOpacity(1.0f);
        painter->setOpacity(currentOpacity);
        painter->setPen(QColor(10, 10, 10, 200));
        QRectF shadowR = nameRect.adjusted(6, 6, -4, 0);
        QRectF textR = nameRect.adjusted(5, 5, -5, 0);
        painter->drawText(shadowR, Qt::TextSingleLine, thumbnail->name, &shadowR);
        painter->setPen(QColor(250, 250, 255, 255));
        painter->drawText(textR, Qt::TextSingleLine, thumbnail->name, &textR);
        // label with additional info
        if(showLabel) {
            painter->setFont(fontsmall);
            QPointF labelTextPos = labelRect.bottomLeft() + QPointF(3, -6);
            painter->setPen(QColor(10, 10, 10, 255));
            painter->setPen(QColor(160, 160, 160, 255));
            painter->drawText(labelTextPos, thumbnail->label);
        }
    }
}

QSizeF ThumbnailLabel::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    switch(which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
            return QSize(thumbnailSize, thumbnailSize)
                   + QSize(borderSize * 2, borderSize * 2);
        case Qt::MaximumSize:
            return QSize(thumbnailSize, thumbnailSize)
                   + QSize(borderSize * 2, borderSize * 2);
        default:
            break;
    }
    return constraint;
}

void ThumbnailLabel::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    hovered = true;
    if(opacityAnimation->state() == QAbstractAnimation::Running)
        opacityAnimation->stop();
    setOpacity(1.0f);
}

void ThumbnailLabel::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    hovered = false;
    if(!this->isHighlighted())
        this->setOpacityAnimated(0.67, 100);
        //setOpacity(0.6f);
}

ThumbnailLabel::~ThumbnailLabel() {
    delete thumbnail;
    delete highlightColor;
    delete outlineColor;
    delete nameColor;
    delete labelColor;
    delete fm;
}
