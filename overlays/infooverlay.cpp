#include "infooverlay.h"

InfoOverlay::InfoOverlay(QWidget *parent) : OverlayWidget(parent)
{
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    font.setPixelSize(11);
    font.setBold(true);
    textMarginX = 8;
    textMarginY = 4;
    textColor.setRgb(255, 255, 255, 255);
    textShadowColor.setRgb(0, 0, 0, 200);
    bgColor.setRgb(0, 0, 0, 90);
    fm = new QFontMetrics(font);
    setText("No file opened.");
    hide();
}

void InfoOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    if(!text.isEmpty()) {
        QPainter painter(this);
        painter.fillRect(geometry(), QBrush(bgColor));
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
