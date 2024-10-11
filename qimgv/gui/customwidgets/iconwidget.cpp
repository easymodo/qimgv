#include "iconwidget.h"

IconWidget::IconWidget(QWidget *parent)
    : QWidget(parent),
      hiResPixmap(false),
      pixmap(nullptr)
{
    dpr = this->devicePixelRatioF();
    color = settings->colorScheme().icons;
    connect(settings, &Settings::settingsChanged, this, &IconWidget::onSettingsChanged);
}

IconWidget::~IconWidget() {
    if(pixmap)
        delete pixmap;
}

void IconWidget::onSettingsChanged() {
    if(colorMode == ICON_COLOR_THEME && color != settings->colorScheme().icons) {
        color = settings->colorScheme().icons;
        applyColor();
    }
}

void IconWidget::setIconPath(QString path) {
    if(iconPath == path)
        return;
    iconPath = path;
    loadIcon();
}

void IconWidget::loadIcon() {
    auto path = iconPath;
    if(pixmap)
        delete pixmap;
    if(dpr >= (1.0 + 0.001)) {
        path.replace(".", "@2x.");
        hiResPixmap = true;
        pixmap = new QPixmap(path);
        if(dpr >= (2.0 - 0.001))
            pixmapDrawScale = dpr;
        else
            pixmapDrawScale = 2.0;
        pixmap->setDevicePixelRatio(pixmapDrawScale);
    } else {
        hiResPixmap = false;
        pixmap = new QPixmap(path);
        pixmapDrawScale = dpr;
    }
    applyColor();
    if(pixmap->isNull()) {
        delete pixmap;
        pixmap = nullptr;
    }
    update();
}

QSize IconWidget::minimumSizeHint() const {
    if(pixmap && !pixmap->isNull())
        return pixmap->size() / dpr;
    else
        return QWidget::minimumSizeHint();
}

void IconWidget::setIconOffset(int x, int y) {
    iconOffset.setX(x);
    iconOffset.setY(y);
    update();
}

void IconWidget::setColorMode(IconColorMode _mode) {
    if(colorMode != _mode && _mode == ICON_COLOR_SOURCE) {
        colorMode = _mode;
        // reload uncolored
        loadIcon();
    } else {
        colorMode = _mode;
        applyColor();
    }
}

void IconWidget::setColor(QColor _color) {
    colorMode = ICON_COLOR_CUSTOM;
    color = _color;
    applyColor();
}

void IconWidget::applyColor() {
    if(!pixmap || pixmap->isNull() || colorMode == ICON_COLOR_SOURCE)
        return;
    ImageLib::recolor(*pixmap, color);
}

void IconWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter p(this);
    if(!this->isEnabled())
        p.setOpacity(0.5f);
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    if(pixmap) {
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        QPointF pos;
        if(hiResPixmap) {
            pos = QPointF(width()  / 2 - pixmap->width()  / (2 * pixmapDrawScale),
                          height() / 2 - pixmap->height() / (2 * pixmapDrawScale));
        } else {
            pos = QPointF(width()  / 2 - pixmap->width()  / 2,
                          height() / 2 - pixmap->height() / 2);
        }
        p.drawPixmap(pos + iconOffset, *pixmap);
    }
}
