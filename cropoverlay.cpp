#include "cropoverlay.h"

CropOverlay::CropOverlay(QWidget *parent) :
    QWidget(parent)
{
    viewer = parent;
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    startPos = QPoint(0,0);
    endPos = QPoint(0,0);
    imageArea.setRect(0,0,0,0);
    clear = true;
    this->hide();
}

void CropOverlay::setImageArea(QRect area) {
    imageArea = area;
    startPos = QPoint(0,0);
    endPos = QPoint(0,0);
    update();
}

void CropOverlay::display() {
    this->setGeometry(viewer->rect());
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
    this->show();
}

void CropOverlay::hide() {
    clear=true;
    update();
    startPos = QPoint(0,0);
    endPos = QPoint(0,0);
    imageArea.setRect(0,0,0,0);
    selectionRect.setRect(0,0,0,0);
    this->clearFocus();
    QWidget::hide();
}

void CropOverlay::paintEvent(QPaintEvent *event) {
    this->setGeometry(viewer->rect());
    QPainter painter(this);
    QBrush brushDark(QColor(10,10,10,180)); // transparent black
    QBrush brushGray(QColor(80,80,80,180)); // transparent gray

    // draw label
    if(clear) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(brushGray);
        painter.drawRect(7,23,77,20);
        QFont font;
        font.setPixelSize(13);
        painter.setFont(font);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor(10,205,10,255)));
        painter.drawText(QRect(10,25,100,16), "CROP MODE");
        return;
    }

    qDebug() << "selection" << selectionRect;
    qDebug() << "this" << rect();
    qDebug() << "imgArea" << imageArea;

    // draw selection
    painter.setBrush(brushDark);
    painter.setPen(Qt::NoPen);
    painter.drawRect(imageArea.left(), imageArea.top(),
                     selectionRect.left()-imageArea.left(), imageArea.height()); // left
    painter.drawRect(selectionRect.right()+1, imageArea.top(),
                     imageArea.right()-selectionRect.right(), imageArea.height()); // right
    painter.drawRect(selectionRect.left(), imageArea.top(),
                     selectionRect.width(), selectionRect.top()-imageArea.top()); // cutout top
    painter.drawRect(selectionRect.left(), selectionRect.bottom()+1,
                     selectionRect.width(), imageArea.bottom() - selectionRect.bottom()); // cutout bottom
}

// takes QPoint and puts it inside imageArea
QPoint CropOverlay::setInsidePoint(QPoint p) {
    if(p.x()<imageArea.left()) { p.setX(imageArea.left()); }
    else if(p.x()>imageArea.right()) { p.setX(imageArea.right()); }
    if(p.y()<imageArea.top()) { p.setY(imageArea.top()); }
    else if(p.y()>imageArea.bottom()) { p.setY(imageArea.bottom()); }
    return p;
}

void CropOverlay::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        clear=false;
        startPos = setInsidePoint(event->pos());
        endPos = setInsidePoint(event->pos());
        selectionRect.setTopLeft(startPos);
        selectionRect.setBottomRight(endPos);
    } else {
        clear=true;
    }
    update();
}


void CropOverlay::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton && !clear) {
        endPos = setInsidePoint(event->pos());

        //set exclusion rectangle
        QPoint tl, br;
        startPos.x()>=endPos.x()?tl.setX(endPos.x()):tl.setX(startPos.x());
        startPos.y()>=endPos.y()?tl.setY(endPos.y()):tl.setY(startPos.y());

        startPos.x()<=endPos.x()?br.setX(endPos.x()):br.setX(startPos.x());
        startPos.y()<=endPos.y()?br.setY(endPos.y()):br.setY(startPos.y());

        selectionRect.setTopLeft(tl);
        selectionRect.setBottomRight(br);
        qDebug() << "EX:" << selectionRect;
        update();
    }
}

void CropOverlay::mouseReleaseEvent(QMouseEvent *event) {
    if(startPos==endPos) {
        clear=true;
        update();
    }
    //update();
}

void CropOverlay::keyPressEvent(QKeyEvent *event)
{
    qDebug() << event->key();
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return && !clear) {
        qDebug() << "CROP selected size: " << selectionRect.size() << "*scale";
        clear=true;
        update();
    } else if(event->key() == Qt::Key_Escape) {
        this->hide();
    } else {
        event->ignore();
    }
}
