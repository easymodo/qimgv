#include "infooverlay.h"

textOverlay::textOverlay(QWidget *parent) :QWidget(parent) {
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    currentText = "No file opened.";
    font.setPixelSize(13);
    drawRect.setTopLeft(QPoint(4,2));
    drawRect.setBottomRight(QPoint(950,17));
    textColor = new QColor(255,255,255,255);
    textShadowColor = new QColor(0,0,0,150);
    this->setFixedHeight(19);
    this->hide();
    updateWidth();
    updatePosition();
}

void textOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);    
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(*textShadowColor));
    painter.drawText(drawRect.adjusted(1,1,1,1), currentText);
    painter.setPen(QPen(*textColor));
    painter.drawText(drawRect, currentText);
}

void textOverlay::setText(QString text) {
    currentText = text;
    this->update();
}

void textOverlay::updateWidth() {
    this->setMinimumWidth(parentWidget()->width()-60);
}

void textOverlay::updatePosition() {
    QRect newPos = rect();
    newPos.moveTop(0);
    setGeometry(newPos);
    update();
}
