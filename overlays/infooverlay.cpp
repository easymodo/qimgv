#include "infooverlay.h"

textOverlay::textOverlay(QWidget *parent) : QWidget(parent), textLength(0) {
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    currentText = "No file opened.";
    font.setPixelSize(11);
    font.setBold(true);
    drawRect.setTopLeft(QPoint(2, 2));
    drawRect.setBottomRight(QPoint(950, 19));
    textColor = new QColor(255, 255, 255, 255);
    textShadowColor = new QColor(0, 0, 0, 200);
    fm = new QFontMetrics(font);
    this->setFixedHeight(20);
    this->hide();
    //updateWidth();
    //updatePosition();
}

void textOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(QRect(0,0,textLength+8,22), QBrush(QColor(0, 0, 0, 80), Qt::SolidPattern));
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(*textShadowColor));
    painter.drawText(drawRect.adjusted(1, 1, 1, 1), currentText);
    painter.setPen(QPen(*textColor));
    painter.drawText(drawRect, currentText);
}

void textOverlay::setText(QString text) {
    currentText = text;
    textLength = fm->width(currentText);
    this->update();
}

void textOverlay::updateWidth(int maxWidth) {
    this->setMinimumWidth(maxWidth - 60);
}

void textOverlay::updatePosition() {
    QRect newPos = rect();
    newPos.moveTop(0);
    setGeometry(newPos);
    update();
}
