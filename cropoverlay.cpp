#include "cropoverlay.h"

CropOverlay::CropOverlay(QWidget *parent) : QWidget(parent),
    viewer(parent),
    startPos(QPoint(0,0)),
    endPos(QPoint(0,0)),
    imageArea(QRect(0,0,0,0)),
    selectionRect(QRect(0,0,0,0)),
    clear(true),
    moving(false)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->hide();
}

void CropOverlay::setImageArea(QRect area) {
    imageArea = area;
    startPos = QPoint(0,0);
    endPos = QPoint(0,0);
    clearSelection();
    update();
}

void CropOverlay::clearSelection() {
    selectionRect.setTopLeft(imageArea.topLeft());
    selectionRect.setWidth(0);
    selectionRect.setHeight(0);
}

void CropOverlay::display() {
    this->setCursor(Qt::ArrowCursor);
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
    clearSelection();
    this->clearFocus();
    QWidget::hide();
}

void CropOverlay::paintEvent(QPaintEvent *event) {
    this->setGeometry(viewer->rect());
    QPainter painter(this);
    QBrush brushDark(QColor(10,10,10,180)); // transparent black
    QBrush brushGray(QColor(80,80,80,180)); // transparent gray

    // draw label only
    if(clear) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(brushGray);
        painter.drawRect(10,23,77,20);
        QFont font;
        font.setPixelSize(13);
        painter.setFont(font);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor(10,205,10,255)));
        painter.drawText(QRect(13,25,100,16), "CROP MODE");
        return;
    }

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

// takes QPoint and puts it inside rectangle
QPoint CropOverlay::setInsidePoint(QPoint p, QRect area) {
    if(p.x()<area.left()) { p.setX(area.left()); }
    else if(p.x()>area.right()) { p.setX(area.right()); }
    if(p.y()<area.top()) { p.setY(area.top()); }
    else if(p.y()>area.bottom()) { p.setY(area.bottom()); }
    return p;
}

// moves first QRect inside second
QRect CropOverlay::placeInside(QRect what, QRect where) {
    if(what.left()<where.left()) {
        what.moveLeft(where.left());
    }
    if(what.top()<where.top()) {
        what.moveTop(where.top());
    }
    if(what.right()>where.right()) {
        what.moveRight(where.right());
    }
    if(what.bottom()>where.bottom()) {
        what.moveBottom(where.bottom());
    }
    return what;
}

void CropOverlay::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        if(selectionRect.contains(event->pos()) && !moving) { // move selection
            moving=true;
            moveStartPos = QCursor::pos(); // just absolute pos to avoid headache
        }
        else { // select
            clear=false;
            startPos = setInsidePoint(event->pos(), imageArea);
            endPos = setInsidePoint(event->pos(), imageArea);
            selectionRect.setTopLeft(startPos);
            selectionRect.setBottomRight(endPos);
        }
    } else {
        clearSelection();
        clear=true;
    }
    update();
}

void CropOverlay::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton && !clear) {
        if(moving) {
            moving = true;
            QPoint delta = QCursor::pos() - moveStartPos;
            QRect tmp = selectionRect.translated(delta);
            if(!imageArea.contains(tmp, false)) {
                tmp = placeInside(tmp, imageArea);
                //qDebug() << "start: " << moveStartPos << "  old: " << QCursor::pos() << "  new: " << mapToGlobal(imageArea.center());
                QCursor::setPos(mapToGlobal(setInsidePoint(event->pos(), tmp)));
            }
            selectionRect = tmp;
            moveStartPos = QCursor::pos();
            update();
        }
        else {
            endPos = setInsidePoint(event->pos(), imageArea);

            //set exclusion rectangle
            QPoint tl, br;
            startPos.x()>=endPos.x()?tl.setX(endPos.x()):tl.setX(startPos.x());
            startPos.y()>=endPos.y()?tl.setY(endPos.y()):tl.setY(startPos.y());

            startPos.x()<=endPos.x()?br.setX(endPos.x()):br.setX(startPos.x());
            startPos.y()<=endPos.y()?br.setY(endPos.y()):br.setY(startPos.y());

            selectionRect.setTopLeft(tl);
            selectionRect.setBottomRight(br);
            //qDebug() << "EX:" << selectionRect;
            update();
        }
    }
}

void CropOverlay::mouseReleaseEvent(QMouseEvent *event) {
    moving = false;
    if(startPos==endPos) {
        clear=true;
        update();
    }
    //update();
}

void CropOverlay::keyPressEvent(QKeyEvent *event)
{
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
