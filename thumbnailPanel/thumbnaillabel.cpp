#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel(QWidget *parent) :
    QLabel(parent),
    orientation(Qt::Horizontal),
    hovered(false),
    loaded(false),
    showLabel(false),
    showName(true),
    state(EMPTY),
    highlighted(false),
    borderW(3),
    borderH(5),
    thumbnailSize(120),
    thumbnail(NULL),
    opacity(1.0f)
{
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
   // this->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
   // setGraphicsItem(this);
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
    if(settings->panelPosition() == LEFT || settings->panelPosition() == RIGHT) {
        orientation = Qt::Vertical;
        borderW = 5;
        borderH = 3;
        highlightRect.setTopLeft(QPointF(thumbnailSize + borderW, borderH));
        highlightRect.setBottomRight(QPointF(borderW * 2 + thumbnailSize, thumbnailSize + borderH));
    } else {
        orientation = Qt::Horizontal;
        borderW = 3;
        borderH = 5;
        highlightRect.setTopLeft(QPointF(borderW, 0));
        highlightRect.setBottomRight(QPointF(borderW + thumbnailSize, borderH));
    }
    this->setFixedSize(thumbnailSize+borderW*2, thumbnailSize+borderH*2);
    nameRect.setTopLeft(QPointF(borderW, borderH));
    nameRect.setBottomRight(QPointF(borderW + thumbnailSize, borderH + 17));
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
//    this->setOffset(QPointF(borderW, borderH));
}

void ThumbnailLabel::applySettings() {
    readSettings();
    update();
}

void ThumbnailLabel::setThumbnail(Thumbnail *_thumbnail) {
    thumbnail = _thumbnail;
    this->setPixmap(*_thumbnail->image);
    loaded = true;
    showLabel = settings->showThumbnailLabels() && !thumbnail->label.isEmpty();
    updateLabelWidth();
}

void ThumbnailLabel::updateLabelWidth() {
    if(showLabel) {
        int labelWidth = fm->width(thumbnail->label);
        labelRect.setWidth(labelWidth + 4);
        labelRect.moveRight(nameRect.right());
    }
}

void ThumbnailLabel::setHighlighted(bool x) {
    bool toRepaint = (highlighted == x);
    highlighted = x;
    if(toRepaint) {
        if(x) {
            labelColor->setAlpha(255);
        } else {
            labelColor->setAlpha(230);
        }
        this->update();
    }
}

bool ThumbnailLabel::isHighlighted() {
    return highlighted;
}

void ThumbnailLabel::setOpacity(qreal amount) {
    if(amount != opacity) {
        opacity = amount;
        update();
    }
}

void ThumbnailLabel::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setOpacity(opacity);
    if(thumbnail) {
        if(thumbnail->image) {
            painter.drawPixmap(borderW, borderH, *thumbnail->image);
        }

        //colored bar and shadow on the top
        if(isHighlighted()) {
            painter.fillRect(highlightRect, *highlightColor);
            //painter->fillRect(shadowRect, *shadowGradient);
        }

        //setup font
        painter.setFont(font);

        //nameLabel
        painter.fillRect(nameRect, *nameColor);
        painter.setPen(QColor(10, 10, 10, 200));
        painter.drawText(nameRect.adjusted(3, 2, 0, 0), thumbnail->name);
        painter.setPen(QColor(255, 255, 255, 255));
        painter.drawText(nameRect.adjusted(2, 1, 0, 0), thumbnail->name);

        //typeLabel
        if(showLabel) {
            painter.fillRect(labelRect, *labelColor);
            QPointF labelTextPos = labelRect.bottomLeft() + QPointF(2, -5);
            painter.setPen(QColor(10, 10, 10, 200));
            painter.drawText(labelTextPos + QPointF(1, 1), thumbnail->label);
            painter.setPen(QColor(255, 255, 255, 255));
            painter.drawText(labelTextPos, thumbnail->label);
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

ThumbnailLabel::~ThumbnailLabel() {

}

