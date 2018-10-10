#include "infooverlay.h"

InfoOverlay::InfoOverlay(OverlayContainerWidget *parent)
    : OverlayWidget(parent),
      fm(nullptr)
{
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    textMarginX = 8;
    textMarginY = 4;
    textColor.setRgb(255, 255, 255, 255);
    textShadowColor.setRgb(0, 0, 0, 200);
    bgColor.setRgb(0, 0, 0, 115);
    font.setBold(true);
    qreal fntSz = font.pointSizeF();
    if(fntSz > 0) {
        font.setPointSizeF(font.pointSizeF() * 0.9);
    }
    fm = new QFontMetrics(font);
    hide();
}

void InfoOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    if(!text.isEmpty()) {
        QPainter painter(this);
        QRect rect = QRect(0,0, width(), height());
        painter.fillRect(rect, QBrush(bgColor));
        painter.setFont(font);
        painter.setPen(QPen(textShadowColor));
        painter.drawText(textRect.adjusted(1,1,1,1), Qt::TextSingleLine, text);
        painter.setPen(QPen(textColor));
        painter.drawText(textRect, Qt::TextSingleLine, text);
    }
}

void InfoOverlay::setText(QString text) {
    this->text = text;
    recalculateGeometry();
    update();
}

void InfoOverlay::recalculateGeometry() {
    QRect newRect = QRect(0, 0, fm->width(text) + textMarginX * 2, fm->height() + textMarginY * 2);
    if(newRect.width() > containerSize().width() - 150)
        newRect.setWidth(containerSize().width() - 150);
    textRect = newRect.adjusted(textMarginX, textMarginY, -textMarginX, -textMarginY);
    setGeometry(newRect);
}
