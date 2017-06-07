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
    marginX(1),
    highlightHeight(3),
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
        //delete old thumbnail
        if(thumbnail) {
            delete thumbnail;
            thumbnail = NULL;
            this->state = EMPTY;
        }
        thumbnailSize = size;
        highlightRect.setTopLeft(QPointF(marginX, 0));
        highlightRect.setBottomRight(QPointF(marginX + thumbnailSize, highlightHeight));
        nameRect.setTopLeft(QPointF(marginX, highlightHeight));
        nameRect.setBottomRight(QPointF(marginX + thumbnailSize,
                                        highlightHeight + 22));
        nameRect.setWidth(thumbnailSize);
        labelRect = QRectF(QPointF(marginX + thumbnailSize - 25,
                                   highlightHeight),
                           QPointF(marginX + thumbnailSize,
                                   highlightHeight + nameRect.height()));
        //qDebug() << "setThumbSize: " << size << nameRect << labelRect;
        updateLabelWidth();
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
        //qDebug() << "setThumbnail:  " << thumbnail->name;
        update();
    }
}

void ThumbnailLabel::updateLabelWidth() {
    if(showLabel && thumbnail) {
        int labelWidth = fmsmall->width(thumbnail->label);
        labelRect.setWidth(labelWidth + 6);
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
        QPropertyAnimation *anim = new QPropertyAnimation(this, "currentOpacity");
        anim->setEasingCurve(QEasingCurve::InQuad);
        anim->setDuration(speed);
        anim->setStartValue(this->opacity());
        anim->setEndValue(amount);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

QRectF ThumbnailLabel::boundingRect() const
{
    return QRectF(0, 0, thumbnailSize+marginX*2, thumbnailSize+highlightHeight);
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
    if(isHighlighted()) {
        if(drawSelectionBorder) {
            painter->setPen(QColor(10, 10, 10, 150));
            painter->drawRect(boundingRect().adjusted(marginX+1, 0, -marginX-2, -highlightHeight-2));
            painter->setPen(*highlightColor);
            painter->drawRect(boundingRect().adjusted(marginX,highlightHeight,-marginX-1,-highlightHeight-1));
        }
        painter->fillRect(highlightRect, *highlightColor);
    } else if(isHovered()) { // lazy copy-paste, whatever
        if(drawSelectionBorder) {
            painter->setPen(QColor(10, 10, 10, 150));
            painter->drawRect(boundingRect().adjusted(marginX+1, 0, -marginX-2, -highlightHeight-2));
            painter->setPen(*hoverHighlightColor);
            painter->drawRect(boundingRect().adjusted(marginX,highlightHeight,-marginX-1,-highlightHeight-1));
        }
        painter->fillRect(highlightRect, *hoverHighlightColor);
    }
    if(!thumbnail) {
        painter->setOpacity(0.95f);
        painter->drawPixmap((width() - loadingIcon->width()) / 2,
                            highlightHeight + (height() - loadingIcon->height() - highlightHeight) / 2,
                            *loadingIcon);
    } else {
        painter->setOpacity(currentOpacity);
        painter->drawPixmap((width() - thumbnail->image->width()) / 2,
                            highlightHeight + (height() - thumbnail->image->height() - highlightHeight) / 2,
                            *thumbnail->image);
        painter->setOpacity(0.95f);
        //setup font
        painter->setFont(font);
        //nameLabel
        painter->fillRect(nameRect, *nameColor);
        painter->setOpacity(1.0f);
        painter->setPen(QColor(10, 10, 10, 200));
        QRectF shadowR = nameRect.adjusted(5, 6, -4, 0);
        QRectF textR = nameRect.adjusted(4, 5, -5, 0);
        painter->drawText(shadowR, Qt::TextSingleLine, thumbnail->name, &shadowR);
        painter->setPen(QColor(250, 250, 255, 255));
        painter->drawText(textR, Qt::TextSingleLine, thumbnail->name, &textR);
        // label with additional info
        if(showLabel) {
            painter->setFont(fontsmall);
            QPointF labelTextPos = labelRect.bottomLeft() + QPointF(3, -7);
            painter->setPen(QColor(10, 10, 10, 255));
            painter->setPen(QColor(160, 160, 170, 255));
            painter->drawText(labelTextPos, thumbnail->label);
        }
    }
}

QSizeF ThumbnailLabel::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    switch(which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
            return QSize(thumbnailSize, thumbnailSize)
                   + QSize(marginX * 2, highlightHeight);
        case Qt::MaximumSize:
            return QSize(thumbnailSize, thumbnailSize)
                   + QSize(marginX * 2, highlightHeight);
        default:
            break;
    }
    return constraint;
}

void ThumbnailLabel::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    hovered = true;
    setOpacity(1.0f);
}

void ThumbnailLabel::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    hovered = false;
    setOpacity(0.86f);
    //this->setOpacityAnimated(0.83, 70);
}

ThumbnailLabel::~ThumbnailLabel() {
    delete thumbnail;
    delete highlightColor;
    delete outlineColor;
    delete nameColor;
    delete labelColor;
    delete fm;
}
