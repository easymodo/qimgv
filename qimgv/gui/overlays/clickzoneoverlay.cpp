#include "clickzoneoverlay.h"

ClickZoneOverlay::ClickZoneOverlay(FloatingWidgetContainer *parent) :
    FloatingWidget(parent)
{
    // this is just for painting, we are handling mouse events elsewhere
    setAttribute(Qt::WA_TransparentForMouseEvents);
    if(parent)
        setContainerSize(parent->size());

    dpr = this->devicePixelRatioF();
    pixmapLeft = loadPixmap(":/res/icons/common/overlay/arrow_left_50.png");
    pixmapRight = loadPixmap(":/res/icons/common/overlay/arrow_right_50.png");

    connect(settings, &Settings::settingsChanged, this, &ClickZoneOverlay::readSettings);
    readSettings();

    this->show();
}

void ClickZoneOverlay::readSettings() {
    if(settings->clickableEdgesVisible() == drawZones)
        return;
    drawZones = settings->clickableEdgesVisible();
    update();
}

QPixmap* ClickZoneOverlay::loadPixmap(QString path) {
    QPixmap *pixmap;
    if(dpr >= (1.0 + 0.001)) {
        path.replace(".", "@2x.");
        hiResPixmaps = true;
        pixmap = new QPixmap(path);
        if(dpr >= (2.0 - 0.001))
            pixmapDrawScale = dpr;
        else
            pixmapDrawScale = 2.0;
        pixmap->setDevicePixelRatio(pixmapDrawScale);
    } else {
        hiResPixmaps = false;
        pixmap = new QPixmap(path);
        pixmapDrawScale = dpr;
    }
    ImageLib::recolor(*pixmap, QColor(255,255,255));
    if(pixmap->isNull()) {
        delete pixmap;
        pixmap = new QPixmap();
    }
    return pixmap;
}

QRect ClickZoneOverlay::leftZone() {
    return mLeftZone;
}

QRect ClickZoneOverlay::rightZone() {
    return mRightZone;
}

void ClickZoneOverlay::highlightLeft() {
    setHighlightedZone(HIGHLIGHT_LEFT);
}

void ClickZoneOverlay::highlightRight() {
    setHighlightedZone(HIGHLIGHT_RIGHT);
}

void ClickZoneOverlay::disableHighlight() {
    setHighlightedZone(HIGHLIGHT_NONE);
}

bool ClickZoneOverlay::isHighlighted() {
    return (activeZone != HIGHLIGHT_NONE);
}

void ClickZoneOverlay::setPressed(bool mode) {
    if(isPressed == mode)
        return;
    isPressed = mode;
    if(isHighlighted())
        update();
}

void ClickZoneOverlay::setHighlightedZone(ActiveHighlightZone zone) {
    activeZone = zone;
    update();
}

void ClickZoneOverlay::recalculateGeometry() {
    setGeometry(0,0, containerSize().width(), containerSize().height());
}

void ClickZoneOverlay::resizeEvent(QResizeEvent *event) {
    mLeftZone = QRect(0,0, zoneSize, height());
    mRightZone = QRect(width() - zoneSize, 0, zoneSize, height());
}

void ClickZoneOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    if(activeZone == HIGHLIGHT_NONE || !drawZones || width() <= 250)
        return;
    QPainter p(this);
    if(isPressed)
        p.setOpacity(0.06f);
    else
        p.setOpacity(0.10f);
    QBrush brush;
    brush.setColor(QColor(200, 200, 200));
    brush.setStyle(Qt::SolidPattern);

    if(activeZone == HIGHLIGHT_LEFT) {
        p.fillRect(mLeftZone, brush);
        drawPixmap(p, pixmapLeft, mLeftZone);
    }
    if(activeZone == HIGHLIGHT_RIGHT) {
        p.fillRect(mRightZone, brush);
        drawPixmap(p, pixmapRight, mRightZone);
    }
}

// draws pixmap centered inside rect
void ClickZoneOverlay::drawPixmap(QPainter &p, QPixmap *pixmap, QRect rect) {
    if(isPressed)
        p.setOpacity(0.37f);
    else
        p.setOpacity(0.5f);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    QPointF pos;
    if(hiResPixmaps) {
        pos = QPointF(rect.left() + rect.width()  / 2 - pixmap->width()  / (2 * pixmapDrawScale),
                      rect.top() + rect.height() / 2 - pixmap->height() / (2 * pixmapDrawScale));
    } else {
        pos = QPointF(rect.left() + rect.width()  / 2 - pixmap->width()  / 2,
                      rect.top() + rect.height() / 2 - pixmap->height() / 2);
    }
    p.drawPixmap(pos, *pixmap);
}
