#include "cropoverlay.h"

CropOverlay::CropOverlay(QWidget *parent) : QWidget(parent),
    viewer(parent),
    startPos(QPoint(0,0)),
    endPos(QPoint(0,0)),
    imageArea(QRect(0,0,0,0)),
    selectionRect(QRect(0,0,0,0)),
    clear(true),
    moving(false),
    scale(1.0f)
{

    brushDark.setColor(QColor(10,10,10,180));  // transparent black
    brushDark.setStyle(Qt::SolidPattern);
    brushGray.setColor(QColor(80,80,80,180)); // transparent gray
    brushGray.setStyle(Qt::SolidPattern);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->hide();
}

void CropOverlay::setRealSize(QSize sz) {
    realSize = sz;
}

void CropOverlay::setImageArea(QRect area, float _scale) {
    clear = true;
    scale = _scale;
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


//###################################################
//##################### PAINT #######################
//###################################################
void CropOverlay::paintEvent(QPaintEvent *event) {
    //this->setGeometry(viewer->rect());
    QPainter painter(this);

    // draw label + tint over image
    if(clear) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(brushDark);
        painter.drawRect(this->imageArea);
        drawLabel("CROP MODE", QPoint(20,25), &painter);
        return;
    }

    //selection outline
    drawAroundSelection(&painter);

    //size label
    QRect r = mapSelection();
    QString info;
    info.append(QString::number(r.width()));
    info.append("x");
    info.append(QString::number(r.height()));
    if(selectionRect.width()<70 || selectionRect.height()<30) {
        drawLabel(info, selectionRect.topRight()+QPoint(1,0), &painter);
    } else {
        drawLabel(info, selectionRect.topLeft(), &painter);
    }
}
//###################################################
//###################################################

void CropOverlay::drawAroundSelection(QPainter *painter) {
    // draw selection
    painter->setBrush(brushDark);
    painter->setPen(Qt::NoPen);
    painter->drawRect(imageArea.left(),
                      imageArea.top(),
                      selectionRect.left()-imageArea.left(),
                      imageArea.height()); // left
    painter->drawRect(selectionRect.right()+1,
                      imageArea.top(),
                      imageArea.right()-selectionRect.right(),
                      imageArea.height()); // right
    painter->drawRect(selectionRect.left(),
                      imageArea.top(),
                      selectionRect.width(),
                      selectionRect.top()-imageArea.top()); // top
    painter->drawRect(selectionRect.left(),
                      selectionRect.bottom()+1,
                      selectionRect.width(),
                      imageArea.bottom() - selectionRect.bottom()); // bottom
}

void CropOverlay::drawLabel(QString text, QPoint pos, QPainter *painter) {
    QFont font;
    font.setPixelSize(12);
    QFontMetrics fm(font);
    int textWidth = fm.width(text);
    int textHeight = fm.height();

    int marginH = 2;
    int marginW = 4;
    painter->setPen(Qt::NoPen);
    painter->setBrush(brushGray);
    painter->drawRect(pos.x(),pos.y(),
                      textWidth+marginW*2,textHeight+marginH*2);

    painter->setFont(font);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(10,205,10,255)));
    painter->drawText(QRect(pos.x()+marginW,pos.y()+marginH,
                            textWidth,textHeight),
                      text);
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
        if(moving) { // moving selection
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
        else { // selecting
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
    qDebug() << mapSelection();
}

void CropOverlay::mouseReleaseEvent(QMouseEvent *event) {
    moving = false;
    if(startPos==endPos) {
        clear=true;
        update();
    }
    //update();
}

QRect CropOverlay::mapSelection() {
    QRect tmp = selectionRect;
    tmp.moveTopLeft((tmp.topLeft()-imageArea.topLeft())/scale);
    tmp.setSize(selectionRect.size()/scale);
    //qDebug() << "CROP selected rect: " << tmp;
    // kinda ugly but works
    if(selectionRect.top() == imageArea.top()) {
        tmp.setTop(0);
    }
    if(selectionRect.left() == imageArea.left()) {
        tmp.setLeft(0);
    }
    if(selectionRect.bottom() == imageArea.bottom()) {
        tmp.setBottom(realSize.height()-1);
    }
    if(selectionRect.right() == imageArea.right()) {
        tmp.setRight(realSize.width()-1);
    }
    qDebug() << "CROP after correction: " << tmp;
    return tmp;
}

void CropOverlay::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return && !clear) {
        emit cropSelected(mapSelection());
        this->hide();
        update();
    } else if(event->key() == Qt::Key_Escape) {
        this->hide();
    } else if (event->matches(QKeySequence::SelectAll)){
        clear = false;
        selectionRect = imageArea;
        update();
    } else {
        event->ignore();
    }
}
