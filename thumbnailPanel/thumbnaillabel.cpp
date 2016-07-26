#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel(QWidget *parent) :
    QLabel(parent),
    state(EMPTY),
    orientation(Qt::Horizontal),
    hovered(false),
    loaded(false),
    showLabel(false),
    showName(true),
    thumbnail(NULL),
    highlighted(false),
    borderW(1),
    borderH(5),
    thumbnailSize(120),
    currentOpacity(1.0f)
{
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    highlightColor = new QColor();
    highlightColorBorder = new QColor(20, 26, 17);    //unused for now
    outlineColor = new QColor(Qt::black);
    nameColor = new QColor(10, 10, 10, 230);
    labelColor = new QColor();
    shadowGradient = new QLinearGradient();

    readSettings();

    font.setPixelSize(11);
    fm = new QFontMetrics(font);
}

// call manually from thumbnailStrip
void ThumbnailLabel::readSettings() {
    thumbnailSize = settings->thumbnailSize();
    if(settings->panelPosition() == LEFT) {
        orientation = Qt::Vertical;
        borderW = 4;
        borderH = 1;
        highlightRect.setTopLeft(QPointF(thumbnailSize + borderW, borderH));
        highlightRect.setBottomRight(QPointF(borderW * 2 + thumbnailSize, thumbnailSize + borderH));
    } else if(settings->panelPosition() == RIGHT) {
            orientation = Qt::Vertical;
            borderW = 4;
            borderH = 1;
            highlightRect.setTopLeft(QPointF(0, borderH));
            highlightRect.setBottomRight(QPointF(borderW, thumbnailSize + borderH));
    } else {
        orientation = Qt::Horizontal;
        borderW = 1;
        borderH = 4;
        highlightRect.setTopLeft(QPointF(borderW, 0));
        highlightRect.setBottomRight(QPointF(borderW + thumbnailSize, borderH));
    }
    this->setFixedSize(thumbnailSize+borderW*2, thumbnailSize+borderH*2);
    nameRect.setTopLeft(QPointF(borderW, borderH));
    nameRect.setBottomRight(QPointF(borderW + thumbnailSize, borderH + 19));
    nameRect.setWidth(thumbnailSize);
    labelRect = QRectF(QPointF(borderW + thumbnailSize - 25, borderH),
                       QPointF(borderW + thumbnailSize, borderH + nameRect.height()));
    updateLabelWidth();
    shadowRect.setTopLeft(QPointF(borderW, borderH));
    shadowRect.setBottomRight(QPointF(borderW + thumbnailSize, borderH + nameRect.height()));

    highlightColor->setRgb(settings->accentColor().rgb());
    labelColor->setRgb(settings->accentColor().rgb());

    shadowGradient->setStart(shadowRect.topLeft());
    shadowGradient->setFinalStop(shadowRect.bottomLeft());
    shadowGradient->setColorAt(0, QColor(0, 0, 0, 100));
    shadowGradient->setColorAt(1, QColor(0, 0, 0, 0));
}

void ThumbnailLabel::applySettings() {
    readSettings();
    update();
}

void ThumbnailLabel::setThumbnail(Thumbnail *_thumbnail) {
    if(_thumbnail) {
        thumbnail = _thumbnail;
        this->setPixmap(*_thumbnail->image);
        loaded = true;
        showLabel = settings->showThumbnailLabels() && !thumbnail->label.isEmpty();
        updateLabelWidth();
        float widthFactor = fm->width(thumbnail->name) / (float)(thumbnailSize - 6);
        if(widthFactor > 1) {
            thumbnail->name.truncate(thumbnail->name.length()/widthFactor);
        }
    }
}

void ThumbnailLabel::updateLabelWidth() {
    if(showLabel && thumbnail) {
        int labelWidth = fm->width(thumbnail->label);
        labelRect.setWidth(labelWidth + 4);
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


void ThumbnailLabel::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    if(thumbnail) {
        if(thumbnail->image) {
            if(hovered)
                painter.setOpacity(1.0f);
            else
                painter.setOpacity(currentOpacity);
            painter.drawPixmap(borderW, borderH, *thumbnail->image);
        }

        painter.setOpacity(0.9f);

        //setup font
        painter.setFont(font);

        //nameLabel
        painter.fillRect(nameRect, *nameColor);
        painter.setPen(QColor(10, 10, 10, 200));
        painter.drawText(nameRect.adjusted(4, 3, 0, 0), thumbnail->name);
        painter.setPen(QColor(255, 255, 255, 255));
        painter.drawText(nameRect.adjusted(3, 2, 0, 0), thumbnail->name);

        painter.setOpacity(1.0f);
        //typeLabel
        if(showLabel) {
            painter.fillRect(labelRect, *labelColor);
            QPointF labelTextPos = labelRect.bottomLeft() + QPointF(2, -6);
            painter.setPen(QColor(10, 10, 10, 200));
            painter.drawText(labelTextPos + QPointF(1, 1), thumbnail->label);
            painter.setPen(QColor(255, 255, 255, 255));
            painter.drawText(labelTextPos, thumbnail->label);
        }

        //colored bar and shadow on the top
        if(isHighlighted()) {
            painter.fillRect(highlightRect, *highlightColor);
            //painter->fillRect(shadowRect, *shadowGradient);
        }
    }
}

QSizeF ThumbnailLabel::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    switch(which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
            return QSize(thumbnailSize, thumbnailSize)
                   + QSize(borderW * 2, borderH * 2);
        case Qt::MaximumSize:
            return QSize(thumbnailSize, thumbnailSize)
                   + QSize(borderW * 2, borderH * 2);
        default:
            break;
    }
    return constraint;
}

void ThumbnailLabel::enterEvent(QEvent *event) {
    hovered = true;
    update();
    event->ignore();
}

void ThumbnailLabel::leaveEvent(QEvent *event) {
    hovered = false;
    update();
    event->ignore();
}

ThumbnailLabel::~ThumbnailLabel() {
    delete thumbnail;
    delete highlightColor;
    delete outlineColor;
    delete highlightColorBorder;
    delete nameColor;
    delete labelColor;
    delete fm;
}

