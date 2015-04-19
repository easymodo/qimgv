#include "infooverlay.h"

textOverlay::textOverlay(QWidget *parent) :QWidget(parent) {
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    currentText = "No file opened.";
    this->setFixedHeight(18);
    this->hide();
    updateWidth();
    updatePosition();
}

void textOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    QFont font;
    font.setPixelSize(13);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QColor(10,205,10,255)));
    painter.drawText(QRect(4,2,950,16), currentText);
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
