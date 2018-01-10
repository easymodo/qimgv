#include "cropoverlay.h"

// TODO: this is pretty old. Clean up

CropOverlay::CropOverlay(ContainerWidget *parent) : OverlayWidget(parent),
    viewer(parent),
    startPos(QPoint(0, 0)),
    endPos(QPoint(0, 0)),
    imageRect(QRect(0, 0, 0, 0)),
    selectionRect(QRect(0, 0, 0, 0)),
    clear(true),
    moving(false),
    scale(1.0f),
    handleSize(8),
    drawBuffer(NULL),
    dragMode(NO_DRAG)
{
    prepareDrawElements();
    brushInactiveTint.setColor(QColor(0, 0, 0, 150));
    brushInactiveTint.setStyle(Qt::SolidPattern);
    brushDarkGray.setColor(QColor(120, 120, 120, 160));
    brushDarkGray.setStyle(Qt::SolidPattern);
    brushGray.setColor(QColor(150, 150, 150, 160));
    brushGray.setStyle(Qt::SolidPattern);
    brushLightGray.setColor(QColor(220, 220, 220, 160));
    brushLightGray.setStyle(Qt::SolidPattern);
    selectionOutlinePen.setColor(Qt::white);
    selectionOutlinePen.setStyle(Qt::SolidLine);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->hide();
}

void CropOverlay::prepareDrawElements() {
    for(int i = 0; i < 8; i++) {
        handles[i] = new QRectF(0, 0, handleSize * 2, handleSize * 2);
    }
}

void CropOverlay::setImageRealSize(QSize sz) {
    imageRealSize = sz;
    clearSelection();
}

void CropOverlay::setImageRect(QRectF imageRect) {
    if(this->imageRect != imageRect) {
        this->imageRect = imageRect;
        clearSelection();
    }
}

void CropOverlay::setImageScale(float scale) {
    this->scale = scale;
    clearSelection();
}

void CropOverlay::clearSelection() {
    if(!clear) {
        startPos = QPoint(0, 0);
        endPos = QPoint(0, 0);
        selectionRect.setTopLeft(imageRect.topLeft());
        selectionRect.setWidth(0);
        selectionRect.setHeight(0);
        clear = true;
        update();
        onSelectionChanged();
    }
}

void CropOverlay::selectAll() {
    clear = false;
    selectionRect = imageRect;
    updateHandlePositions();
    update();
    onSelectionChanged();
}

void CropOverlay::hide() {
    startPos = QPoint(0, 0);
    endPos = QPoint(0, 0);
    imageRect.setRect(0, 0, 0, 0);
    clearSelection();
    this->clearFocus();
    QWidget::hide();
}

void CropOverlay::show() {
    this->setCursor(Qt::ArrowCursor);
    this->setGeometry(viewer->rect());
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
    QWidget::show();
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
        painter.drawRect(imageRect);
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
        painter.drawRect(imageRect);

        // selection outline & handles
        if(selectionRect.width() > 1 && selectionRect.height() > 1) {
            drawSelection(&painter);
            // draw handles if there is no interaction going on
            // and selection is large enough
            if(dragMode == NO_DRAG && selectionRect.width() >= 100 && selectionRect.height() >= 100)
                drawHandles(brushGray, &painter);
        }

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

// takes QPoint and puts it inside rectangle
QPointF CropOverlay::setInsidePoint(QPoint p, QRectF area) {
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
QRectF CropOverlay::placeInside(QRectF what, QRectF where) {
    if(what.width() > where.width()) {
        what.setLeft(where.left());
        what.setRight(where.right());
    } else {
        if(what.left() < where.left())
            what.moveLeft(where.left());
        if(what.right() > where.right())
            what.moveRight(where.right());
    }
    if(what.height() > where.height()) {
        what.setTop(where.top());
        what.setBottom(where.bottom());
    } else {
        if(what.top() < where.top())
            what.moveTop(where.top());
        if(what.bottom() > where.bottom())
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
    else if(selectionRect.contains(pos)) dragMode = DRAG_MOVE;
    else dragMode = NO_DRAG;
}

// returns true if resize succeeded
bool CropOverlay::resizeSelection(QPointF d) {
    QRectF tmp = selectionRect;
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
    // crop selection so it's bounded by imageRect
    selectionRect = selectionRect.intersected(imageRect);
    if(!selectionRect.isValid()) {
        selectionRect = tmp;
        return false;
    }
    return true;
}

void CropOverlay::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        detectClickTarget(event->pos());
        if(selectionRect.contains(event->pos()) && !moving) {       // move selection
            moveStartPos = QCursor::pos(); // just absolute pos to avoid headache
        } else { // start selection
            // TODO: ignore when clicked outside of selected rectangle?
            // unless mouse is moved
            clear = false;
            startPos = setInsidePoint(event->pos(), imageRect);
            endPos = setInsidePoint(event->pos(), imageRect);
            selectionRect.setTopLeft(startPos);
            selectionRect.setBottomRight(endPos);
            onSelectionChanged();
        }
    } else {
        clearSelection();
    }
    update();
}

void CropOverlay::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton && !clear) {
        QPointF delta = QPointF(QCursor::pos()) - moveStartPos;
        if(dragMode == DRAG_MOVE) {    // moving selection
            selectionRect.translate(delta);
            if(!imageRect.contains(selectionRect)) {
                selectionRect = placeInside(selectionRect, imageRect);
            }
            moveStartPos = QCursor::pos();
            updateHandlePositions();
            update();
        } else if(dragMode == NO_DRAG) {    // selecting
            endPos = setInsidePoint(event->pos(), imageRect);

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
                if(!imageRect.contains(selectionRect)) {
                    selectionRect = placeInside(selectionRect, imageRect);
                }
                updateHandlePositions();
                update();
            }
        }
        onSelectionChanged();
    }
}

void CropOverlay::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    dragMode = NO_DRAG;
    update();
}

void CropOverlay::updateHandlePositions() {
    // top left
    handles[0]->moveTopLeft(selectionRect.topLeft() - QPointF(1, 1));
    // top right
    handles[1]->moveTopRight(selectionRect.topRight() - QPointF(0, 1));
    // bottom left
    handles[2]->moveBottomLeft(selectionRect.bottomLeft() - QPointF(1, 0));
    // bottom right
    handles[3]->moveBottomRight(selectionRect.bottomRight());
    // left
    handles[4]->moveTopLeft(QPoint(selectionRect.left() - 1,
                                   selectionRect.center().y() - handleSize));
    // right
    handles[5]->moveTopRight(QPoint(selectionRect.right(),
                                    selectionRect.center().y() - handleSize));
    // top
    handles[6]->moveTopLeft(QPoint(selectionRect.center().x() - handleSize,
                                   selectionRect.top() - 1));
    // bottom
    handles[7]->moveBottomLeft(QPoint(selectionRect.center().x() - handleSize,
                                      selectionRect.bottom()));
}

// map coordinates to imageRect
// TODO: deal with precision issues
// (prevent unwanted selection size change during mapping)
void CropOverlay::onSelectionOutsideChange(QRect unmapped) {
    clear = false;
    QRectF tmp = unmapped;
    tmp.moveLeft(tmp.left()*scale + imageRect.left());
    tmp.moveTop(tmp.top()*scale + imageRect.top());
    tmp.setWidth(tmp.width() * scale);
    tmp.setHeight(tmp.height() * scale);
    selectionRect = tmp;
    updateHandlePositions();
    update();
}

// notify whoever cares about selection changes
void CropOverlay::onSelectionChanged() {
    emit selectionChanged(mapSelection());
}

// translates selection rect into image coordinates
// also performs some sanity checks
QRect CropOverlay::mapSelection() {
    QRectF tmp;
    if(selectionRect.width() == 0 || selectionRect.height() == 0) {
        tmp = QRect(0,0,0,0);
    } else {
        tmp = selectionRect;
        tmp.moveTopLeft((tmp.topLeft() - imageRect.topLeft()) / scale);
        tmp.setSize(selectionRect.size() / scale);
        // kinda ugly but works
        if(selectionRect.top() <= imageRect.top()) {
            tmp.moveTop(0);
        }
        if(selectionRect.left() <= imageRect.left()) {
            tmp.moveLeft(0);
        }
        if(selectionRect.bottom() >= imageRect.bottom()) {
            tmp.moveBottom(imageRealSize.height() - 1);
        }
        if(selectionRect.right() >= imageRect.right()) {
            tmp.moveRight(imageRealSize.width() - 1);
        }
    }
    return QRect(tmp.topLeft().toPoint(), tmp.size().toSize());
}

void CropOverlay::keyPressEvent(QKeyEvent *event) {
    /*if( (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) && !clear) {
        emit selected(mapSelection());
        this->hide();
    } else if(event->key() == Qt::Key_Escape) {
        this->hide();
    } else if(event->matches(QKeySequence::SelectAll)) {
        selectAll();
    } else {
        event->ignore();
    }
    */
    if(event->matches(QKeySequence::SelectAll))
        selectAll();
    else
        event->ignore();
}

void CropOverlay::recalculateGeometry() {
    setGeometry(0,0, containerSize().width(), containerSize().height());
}
