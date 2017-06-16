#include "cropoverlay.h"

CropOverlay::CropOverlay(QWidget *parent) : QWidget(parent),
    viewer(parent),
    startPos(QPoint(0, 0)),
    endPos(QPoint(0, 0)),
    imageArea(QRect(0, 0, 0, 0)),
    selectionRect(QRect(0, 0, 0, 0)),
    clear(true),
    moving(false),
    scale(1.0f),
    handleSize(7),
    drawBuffer(NULL),
    dragMode(NO_DRAG)
{
    //setAttribute(Qt::WA_TranslucentBackground);
    font.setPixelSize(12);
    font.setBold(true);
    fm = new QFontMetrics(font);
    setButtonText("SELECT");

    prepareDrawElements();
    brushInactiveTint.setColor(QColor(50, 50, 50, 200));
    brushInactiveTint.setStyle(Qt::SolidPattern);
    brushDarkGray.setColor(QColor(120, 120, 120, 230));
    brushDarkGray.setStyle(Qt::SolidPattern);
    brushGray.setColor(QColor(150, 150, 150, 255));
    brushGray.setStyle(Qt::SolidPattern);
    brushLightGray.setColor(QColor(220, 220, 220, 230));
    brushLightGray.setStyle(Qt::SolidPattern);
    selectionOutlinePen.setColor(Qt::black);
    selectionOutlinePen.setStyle(Qt::SolidLine);
    labelOutlinePen.setColor(QColor(60, 60, 60, 230));
    labelOutlinePen.setStyle(Qt::SolidLine);
    labelOutlinePen.setWidth(2);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->hide();
}

void CropOverlay::prepareDrawElements() {
    for(int i = 0; i < 8; i++) {
        handles[i] = new QRect(0, 0, handleSize * 2, handleSize * 2);
    }
}

void CropOverlay::setRealSize(QSize sz) {
    realSize = sz;
}

void CropOverlay::setButtonText(QString text) {
    buttonText = text;
    buttonRect = QRect(QPoint(15, 75),
                       QSize(fm->width(buttonText) + textMarginW * 2,
                             fm->height() + textMarginH * 2));
}

void CropOverlay::setImageArea(QRect area, float _scale) {
    clear = true;
    scale = _scale;
    imageArea = area;
    startPos = QPoint(0, 0);
    endPos = QPoint(0, 0);
    //clearSelection();
    update();
}

void CropOverlay::clearSelection() {
    selectionRect.setTopLeft(imageArea.topLeft());
    selectionRect.setWidth(0);
    selectionRect.setHeight(0);
}

void CropOverlay::selectAll() {
    clear = false;
    selectionRect = imageArea;
    updateHandlePositions();
    update();
}

void CropOverlay::display() {
    this->setCursor(Qt::ArrowCursor);
    this->setGeometry(viewer->rect());
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
    selectAll();
    this->show();
}

void CropOverlay::hide() {
    clear = true;
    update();
    startPos = QPoint(0, 0);
    endPos = QPoint(0, 0);
    imageArea.setRect(0, 0, 0, 0);
    clearSelection();
    this->clearFocus();
    QWidget::hide();
}


//###################################################
//##################### PAINT #######################
//###################################################
void CropOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    if(clear) {
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(brushInactiveTint);
        painter.drawRect(imageArea);
        drawLabel(buttonText, buttonRect, brushLightGray, &painter);
        return;

    } else {
        delete drawBuffer;
        drawBuffer = new QImage(size(), QImage::Format_ARGB32_Premultiplied);

        QPainter painter(drawBuffer);

        // clear to avoid corrupted background
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setBrush(brushInactiveTint); // any brush is ok
        painter.drawRect(rect());

        // draw tint over the image
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setPen(Qt::NoPen);
        painter.setBrush(brushInactiveTint);
        painter.drawRect(imageArea);

        // selection outline
        drawSelection(&painter);

        // corner label with selection size
        QRect r = mapSelection();
        QString info;
        info.append(QString::number(r.width()));
        info.append("x");
        info.append(QString::number(r.height()));
        if(selectionRect.width() < 70 || selectionRect.height() < 40) {
            drawLabel(info, selectionRect.topRight() + QPoint(1, 0), brushDarkGray, &painter);
        } else {
            drawLabel(info, selectionRect.topLeft() + QPoint(handleSize * 2, handleSize * 2), brushLightGray, &painter);
            drawHandles(brushGray, &painter);
        }

        drawLabel(buttonText, buttonRect, brushLightGray, &painter);

        // draw result on screen
        QPainter(this).drawImage(QPoint(0, 0), *drawBuffer);
    }
}
//###################################################
//###################################################

void CropOverlay::drawSelection(QPainter *painter) {
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->setPen(selectionOutlinePen);
    painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
    painter->drawRect(selectionRect.marginsAdded(QMargins(1, 1, 0, 0)));
    painter->restore();
}

void CropOverlay::drawHandles(QBrush &brush, QPainter *painter) {
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->setBrush(brush);
    painter->setPen(selectionOutlinePen);
    for(int i = 0; i < 8; i++) {
        painter->drawRect(*handles[i]);
    }
    painter->restore();
}

void CropOverlay::drawLabel(QString text, QPoint pos, QBrush &brush, QPainter *painter) {
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Source);

    int textWidth = fm->width(text);
    int textHeight = fm->height();

    int marginH = 2;
    int marginW = 4;
    QRect labelRect(pos.x(), pos.y() - 1,
                    textWidth + marginW * 2, textHeight + marginH * 2);
    painter->setPen(labelOutlinePen);
    painter->setBrush(brush);
    painter->drawRect(labelRect);

    painter->setFont(font);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawText(QRect(pos.x() + marginW, pos.y() + marginH,
                            textWidth, textHeight),
                      text);
    painter->restore();
}

void CropOverlay::drawLabel(QString text, QRect rect, QBrush &brush, QPainter *painter) {
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Source);

    int textWidth = fm->width(text);
    int textHeight = fm->height();

    painter->setPen(labelOutlinePen);
    painter->setBrush(brush);
    painter->drawRect(rect);

    painter->setFont(font);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawText(QRect(rect.topLeft().x() + textMarginW,
                            rect.topLeft().y() + textMarginH + 1,
                            textWidth, textHeight),
                      text);
    painter->restore();
}

// takes QPoint and puts it inside rectangle
QPoint CropOverlay::setInsidePoint(QPoint p, QRect area) {
    if(p.x() < area.left()) {
        p.setX(area.left());
    } else if(p.x() > area.right()) {
        p.setX(area.right());
    }
    if(p.y() < area.top()) {
        p.setY(area.top());
    } else if(p.y() > area.bottom()) {
        p.setY(area.bottom());
    }
    return p;
}

// moves first QRect inside second
// resizes inner rect to outer size if needed
QRect CropOverlay::placeInside(QRect what, QRect where) {
    if(what.width() > where.width()) {
        what.setWidth(where.width());
    }
    if(what.height() > where.height()) {
        what.setHeight(where.height());
    }
    if(what.left() < where.left()) {
        what.moveLeft(where.left());
    }
    if(what.top() < where.top()) {
        what.moveTop(where.top());
    }
    if(what.right() > where.right()) {
        what.moveRight(where.right());
    }
    if(what.bottom() > where.bottom()) {
        what.moveBottom(where.bottom());
    }
    return what;
}

void CropOverlay::detectClickTarget(QPoint pos) {
    if(handles[0]->contains(pos)) dragMode = DRAG_TOPLEFT;
    else if(handles[1]->contains(pos)) dragMode = DRAG_TOPRIGHT;
    else if(handles[2]->contains(pos)) dragMode = DRAG_BOTTOMLEFT;
    else if(handles[3]->contains(pos)) dragMode = DRAG_BOTTOMRIGHT;
    else if(handles[4]->contains(pos)) dragMode = DRAG_LEFT;
    else if(handles[5]->contains(pos)) dragMode = DRAG_RIGHT;
    else if(handles[6]->contains(pos)) dragMode = DRAG_TOP;
    else if(handles[7]->contains(pos)) dragMode = DRAG_BOTTOM;
    else if(selectionRect.contains(pos)) dragMode = MOVE;
    else dragMode = NO_DRAG;
}

// returns true if resize succeeded
bool CropOverlay::resizeSelection(QPoint d) {
    QRect tmp = selectionRect;
    switch(dragMode) {
        case DRAG_TOPLEFT:
            selectionRect.setTopLeft(selectionRect.topLeft() + d);
            break;
        case DRAG_TOPRIGHT:
            selectionRect.setTopRight(selectionRect.topRight() + d);
            break;
        case DRAG_BOTTOMLEFT:
            selectionRect.setBottomLeft(selectionRect.bottomLeft() + d);
            break;
        case DRAG_BOTTOMRIGHT:
            selectionRect.setBottomRight(selectionRect.bottomRight() + d);
            break;
        case DRAG_LEFT:
            selectionRect.setLeft(selectionRect.left() + d.x());
            break;
        case DRAG_RIGHT:
            selectionRect.setRight(selectionRect.right() + d.x());
            break;
        case DRAG_TOP:
            selectionRect.setTop(selectionRect.top() + d.y());
            break;
        case DRAG_BOTTOM:
            selectionRect.setBottom(selectionRect.bottom() + d.y());
            break;
        default:
            break;
    }
    if(!selectionRect.isValid()) {
        selectionRect = tmp;
        return false;
    }
    return true;
}

void CropOverlay::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        if(buttonRect.contains(event->pos())) {
            emit selected(mapSelection());
            this->hide();
            return;
        }
        detectClickTarget(event->pos());
        if(selectionRect.contains(event->pos()) && !moving) {       // move selection
            moveStartPos = QCursor::pos(); // just absolute pos to avoid headache
        } else { // select
            clear = false;
            startPos = setInsidePoint(event->pos(), imageArea);
            endPos = setInsidePoint(event->pos(), imageArea);
            selectionRect.setTopLeft(startPos);
            selectionRect.setBottomRight(endPos);
        }
    } else {
        clearSelection();
        clear = true;
    }
    update();
}

void CropOverlay::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton && !clear) {
        QPoint delta = QCursor::pos() - moveStartPos;
        if(dragMode == MOVE) {    // moving selection
            QRect tmp = selectionRect.translated(delta);
            if(!imageArea.contains(tmp, false)) {
                tmp = placeInside(tmp, imageArea);
            }
            selectionRect = tmp;
            moveStartPos = QCursor::pos();
            updateHandlePositions();
            update();
        } else if(dragMode == NO_DRAG) {    // selecting
            endPos = setInsidePoint(event->pos(), imageArea);

            //build selection rectangle
            QPoint tl, br;
            startPos.x() >= endPos.x() ? tl.setX(endPos.x()) : tl.setX(startPos.x());
            startPos.y() >= endPos.y() ? tl.setY(endPos.y()) : tl.setY(startPos.y());

            startPos.x() <= endPos.x() ? br.setX(endPos.x()) : br.setX(startPos.x());
            startPos.y() <= endPos.y() ? br.setY(endPos.y()) : br.setY(startPos.y());

            selectionRect.setTopLeft(tl);
            selectionRect.setBottomRight(br);
            updateHandlePositions();
            update();
        } else { // resizing
            if(resizeSelection(delta)) {
                moveStartPos = QCursor::pos();
                if(!imageArea.contains(selectionRect, false)) {
                    selectionRect = placeInside(selectionRect, imageArea);
                }
                updateHandlePositions();
                update();
            }
        }
    }
}

void CropOverlay::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    dragMode = NO_DRAG;
}

void CropOverlay::updateHandlePositions() {
    handles[0]->moveTopLeft(selectionRect.topLeft() - QPoint(1, 1));
    handles[1]->moveTopRight(selectionRect.topRight() - QPoint(0, 1));
    handles[2]->moveBottomLeft(selectionRect.bottomLeft() - QPoint(1, 0));
    handles[3]->moveBottomRight(selectionRect.bottomRight());
    handles[4]->moveTopLeft(
        QPoint(selectionRect.left() - 1, selectionRect.center().y() - handleSize)); // left
    handles[5]->moveTopRight(
        QPoint(selectionRect.right(), selectionRect.center().y() - handleSize));  // right
    handles[6]->moveTopLeft(
        QPoint(selectionRect.center().x() - handleSize, selectionRect.top() - 1)); // top
    handles[7]->moveBottomLeft(
        QPoint(selectionRect.center().x() - handleSize, selectionRect.bottom()));   //bottom
}

QRect CropOverlay::mapSelection() {
    QRect tmp = selectionRect;
    tmp.moveTopLeft((tmp.topLeft() - imageArea.topLeft()) / scale);
    tmp.setSize(selectionRect.size() / scale);
    // kinda ugly but works
    if(selectionRect.top() <= imageArea.top()) {
        tmp.setTop(0);
    }
    if(selectionRect.left() <= imageArea.left()) {
        tmp.setLeft(0);
    }
    if(selectionRect.bottom() >= imageArea.bottom()) {
        tmp.setBottom(realSize.height() - 1);
    }
    if(selectionRect.right() >= imageArea.right()) {
        tmp.setRight(realSize.width() - 1);
    }
    return tmp;
}

void CropOverlay::keyPressEvent(QKeyEvent *event) {
    if( (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) && !clear) {
        emit selected(mapSelection());
        this->hide();
    } else if(event->key() == Qt::Key_Escape) {
        this->hide();
    } else if(event->matches(QKeySequence::SelectAll)) {
        selectAll();
    } else {
        event->ignore();
    }
}

void CropOverlay::mouseDoubleClickEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton && selectionRect.contains(event->pos())) {
        emit selected(mapSelection());
        this->hide();
    } else {
        QWidget::mouseDoubleClickEvent(event);
    }
}
