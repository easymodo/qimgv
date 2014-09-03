#include "infooverlay.h"

InfoOverlay::InfoOverlay(QWidget *parent) :QWidget(parent) {
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    currentText = "No file opened.";
    this->setFixedHeight(30);
    updateSize();
}

void InfoOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QFont font;
    font.setPixelSize(13);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::green));
    painter.drawText(QRect(10,5,950,16),currentText);
}

void InfoOverlay::setText(QString text) {
    currentText = text;
    this->update();
}

void InfoOverlay::updateSize() {
    this->setMinimumWidth(this->parentWidget()->size().rwidth()-60);
}



