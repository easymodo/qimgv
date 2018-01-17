#include "cropoverlay.h"

// TODO: this is pretty old. Clean up

CropOverlay::CropOverlay(ContainerWidget *parent) : OverlayWidget(parent),
    viewer(parent),
    startPos(QPoint(0, 0)),
    endPos(QPoint(0, 0)),
    imageDrawRect(QRect(0, 0, 0, 0)),
    selectionRect(QRect(0, 0, 0, 0)),
    clear(true),
    moving(false),
    scale(1.0f),
    drawBuffer(NULL),
    cursorAction(NO_DRAG)
{
    setMouseTracking(true);
    dpr = devicePixelRatioF();
    handleSize = 8 * dpr;
    prepareDrawElements();
    brushInactiveTint.setColor(QColor(0, 0, 0, 160));
    brushInactiveTint.setStyle(Qt::SolidPattern);
    brushDarkGray.setColor(QColor(120, 120, 120, 160));
    brushDarkGray.setStyle(Qt::SolidPattern);
    brushGray.setColor(QColor(150, 150, 150, 160));
    brushGray.setStyle(Qt::SolidPattern);
    brushLightGray.setColor(QColor(220, 220, 220, 160));
    brushLightGray.setStyle(Qt::SolidPattern);
    selectionOutlinePen.setColor(Qt::white);
    selectionOutlinePen.setStyle(Qt::SolidLine);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    hide();
}

void CropOverlay::prepareDrawElements() {
    for(int i = 0; i < 8; i++) {
        handles[i] = new QRectF(0, 0, handleSize * 2, handleSize * 2);
    }
}

void CropOverlay::setImageRealSize(QSize sz) {
    imageRect.setSize(sz);
    clearSelection();
}

void CropOverlay::setImageDrawRect(QRect imageDrawRect) {
    if(this->imageDrawRect != imageDrawRect) {
        this->imageDrawRect = imageDrawRect;
    }
    clearSelection();
}

void CropOverlay::setImageScale(float scale) {
    this->scale = scale;
    clearSelection();
}

void CropOverlay::clearSelection() {
    if(!clear) {
        startPos = QPoint(0,0);
        endPos = QPoint(0,0);
        selectionRect = QRect(0,0,0,0);
        selectionDrawRect = QRect(0,0,0,0);
        clear = true;
        update();
        emit selectionChanged(selectionRect);
    }
}

void CropOverlay::selectAll() {
    clear = false;
    selectionRect = imageRect;
    updateSelectionDrawRect();
    updateHandlePositions();
    update();
    emit selectionChanged(selectionRect);
}

void CropOverlay::show() {
    if(!drawBuffer) {
        drawBuffer = new QImage(size()*dpr, QImage::Format_ARGB32_Premultiplied);
        drawBuffer->setDevicePixelRatio(dpr);
    }
    QWidget::show();
}

void CropOverlay::hide() {
    startPos = QPoint(0, 0);
    endPos = QPoint(0, 0);
    imageDrawRect.setRect(0, 0, 0, 0);
    clearSelection();
    QWidget::hide();
    clearFocus();
    if(drawBuffer) {
        delete drawBuffer;
        drawBuffer = NULL;
    }
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
        painter.drawRect(QRect(imageDrawRect.topLeft()/dpr, imageDrawRect.size()/dpr));
    } else {
        if(drawBuffer) {
            QPainter painter(drawBuffer);
            // clear to avoid corrupted background
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
            painter.setBrush(brushInactiveTint); // any brush is ok
            painter.drawRect(rect());
            // draw tint over the image
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.setPen(Qt::NoPen);
            painter.setBrush(brushInactiveTint);
            painter.drawRect(imageDrawRect);
            // selection outline & handles
            if(selectionDrawRect.width() > 0 && selectionDrawRect.height() > 0) {
                drawSelection(&painter);
                // draw handles if there is no interaction going on
                // and selection is large enough
                if(cursorAction == NO_DRAG && selectionDrawRect.width() >= 90 && selectionDrawRect.height() >= 90)
                    drawHandles(brushGray, &painter);
            }
            // draw result on screen
            QPainter(this).drawImage(QPoint(0, 0), *drawBuffer);
        }
    }
}
//###################################################
//###################################################

void CropOverlay::drawSelection(QPainter *painter) {
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->setPen(selectionOutlinePen);
    painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
    painter->drawRect(selectionDrawRect.marginsAdded(QMargins(1, 1, 0, 0)));
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

// moves first QRect inside second
// resizes inner rect to outer size if needed
QRect CropOverlay::placeInside(QRect what, QRect where) {
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

CursorAction CropOverlay::hoverTarget(QPoint pos) {
    CursorAction target;
    if(handles[0]->contains(pos)) target = DRAG_TOPLEFT;
    else if(handles[1]->contains(pos)) target = DRAG_TOPRIGHT;
    else if(handles[2]->contains(pos)) target = DRAG_BOTTOMLEFT;
    else if(handles[3]->contains(pos)) target = DRAG_BOTTOMRIGHT;
    else if(handles[4]->contains(pos)) target = DRAG_LEFT;
    else if(handles[5]->contains(pos)) target = DRAG_RIGHT;
    else if(handles[6]->contains(pos)) target = DRAG_TOP;
    else if(handles[7]->contains(pos)) target = DRAG_BOTTOM;
    else if(selectionDrawRect.contains(pos)) target = DRAG_MOVE;
    else target = NO_DRAG;
    return target;
}

void CropOverlay::setCursorForAction(CursorAction action) {
    switch(action) {
        case DRAG_TOPLEFT: setCursor(QCursor(Qt::SizeFDiagCursor)); break;
        case DRAG_TOPRIGHT: setCursor(QCursor(Qt::SizeBDiagCursor)); break;
        case DRAG_BOTTOMLEFT: setCursor(QCursor(Qt::SizeBDiagCursor)); break;
        case DRAG_BOTTOMRIGHT: setCursor(QCursor(Qt::SizeFDiagCursor)); break;
        case DRAG_LEFT: setCursor(QCursor(Qt::SizeHorCursor)); break;
        case DRAG_RIGHT: setCursor(QCursor(Qt::SizeHorCursor)); break;
        case DRAG_TOP: setCursor(QCursor(Qt::SizeVerCursor)); break;
        case DRAG_BOTTOM: setCursor(QCursor(Qt::SizeVerCursor)); break;
        case DRAG_MOVE: setCursor(QCursor(Qt::OpenHandCursor)); break;
        default: setCursor(QCursor(Qt::ArrowCursor)); break;
    }
}

// returns true if resize succeeded
bool CropOverlay::resizeSelection(QPoint d) {
    QRect tmp = selectionRect;
    switch(cursorAction) {
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

// generate a draw rectangle from selectionRect
void CropOverlay::updateSelectionDrawRect() {
    selectionDrawRect = selectionRect;
    selectionDrawRect.moveTopLeft(selectionRect.topLeft() * scale + imageDrawRect.topLeft());
    selectionDrawRect.setSize(selectionRect.size() * scale);
}

// map a point to image coordinate
QPoint CropOverlay::mapPointToImage(QPoint p) {
    // shift relative to (0,0)
    if(p.x()<imageDrawRect.x())
        p.setX(imageDrawRect.x());
    if(p.y()<imageDrawRect.y())
        p.setY(imageDrawRect.y());
    p.setX(p.x() - imageDrawRect.x());
    p.setY(p.y() - imageDrawRect.y());
    p = p / scale;
    // bound to image size
    if(p.x() > imageRect.width())
        p.setX(imageRect.width() - 1);
    if(p.y() > imageRect.height())
        p.setY(imageRect.height() - 1);
    return p;
}

void CropOverlay::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        cursorAction = hoverTarget(event->pos()*dpr);
        setCursorForAction(cursorAction);
        if(cursorAction == NO_DRAG) { // start selection
            clear = false;
            startPos = event->pos();
            endPos = startPos;
            selectionRect.setTopLeft(mapPointToImage(event->pos()*dpr));
            selectionRect.setBottomRight(selectionRect.topLeft());
            updateSelectionDrawRect();
            emit selectionChanged(selectionRect);
        } else {
            moveStartPos = event->pos();
        }
    } else {
        clearSelection();
    }
    update();
}

void CropOverlay::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton && !clear) {
        QPoint delta = (event->pos() - moveStartPos)*dpr;
        if(cursorAction == DRAG_MOVE) { // moving selection
            setCursor(Qt::ClosedHandCursor);
            moveStartPos = event->pos();
            selectionRect.translate(delta / scale);
            if(!imageRect.contains(selectionRect)) {
                selectionRect = placeInside(selectionRect, imageRect);
            }
            updateSelectionDrawRect();
            updateHandlePositions();
            update();
        } else if(cursorAction == NO_DRAG) { // selecting
            endPos = event->pos();
            //build selection rectangle
            QPoint tl, br;
            startPos.x() >= endPos.x() ? tl.setX(endPos.x()) : tl.setX(startPos.x());
            startPos.y() >= endPos.y() ? tl.setY(endPos.y()) : tl.setY(startPos.y());

            startPos.x() <= endPos.x() ? br.setX(endPos.x()) : br.setX(startPos.x());
            startPos.y() <= endPos.y() ? br.setY(endPos.y()) : br.setY(startPos.y());

            selectionRect.setTopLeft(mapPointToImage(tl*dpr));
            selectionRect.setBottomRight(mapPointToImage(br*dpr));
            updateSelectionDrawRect();
            updateHandlePositions();
            update();
        } else { // resizing selection
            if(resizeSelection(delta / scale)) {
                moveStartPos = event->pos();
                if(!imageRect.contains(selectionRect)) {
                    selectionRect = placeInside(selectionRect, imageRect);
                }
                updateSelectionDrawRect();
                updateHandlePositions();
                update();
            }
        }
        emit selectionChanged(selectionRect);
    } else {
        if(clear)
            setCursor(QCursor(Qt::ArrowCursor));
        else
            setCursorForAction(hoverTarget(event->pos()*dpr));
    }
}

void CropOverlay::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    cursorAction = NO_DRAG;
    setCursorForAction(hoverTarget(event->pos()*dpr));
    update();
}

void CropOverlay::updateHandlePositions() {
    // top left
    handles[0]->moveTopLeft(selectionDrawRect.topLeft() + QPointF(-1, -1));
    // top right
    handles[1]->moveTopRight(selectionDrawRect.topRight() + QPointF(1, -1));
    // bottom left
    handles[2]->moveBottomLeft(selectionDrawRect.bottomLeft() + QPointF(-1, 1));
    // bottom right
    handles[3]->moveBottomRight(selectionDrawRect.bottomRight() + QPoint(1, 1));
    // left
    handles[4]->moveTopLeft(QPoint(selectionDrawRect.left() - 1,
                                   selectionDrawRect.center().y() - handleSize));
    // right
    handles[5]->moveTopRight(QPoint(selectionDrawRect.right() + 1,
                                    selectionDrawRect.center().y() - handleSize));
    // top
    handles[6]->moveTopLeft(QPoint(selectionDrawRect.center().x() - handleSize,
                                   selectionDrawRect.top() - 1));
    // bottom
    handles[7]->moveBottomLeft(QPoint(selectionDrawRect.center().x() - handleSize,
                                      selectionDrawRect.bottom() + 1));
}

void CropOverlay::onSelectionOutsideChange(QRect selection) {
    if(selection.width() > 0 && selection.height() > 0) {
        clear = false;
        QRect boundedSelection = placeInside(selection, imageRect);
        selectionRect = boundedSelection;
        updateSelectionDrawRect();
        updateHandlePositions();
        update();
        if(selectionRect != selection)
            emit selectionChanged(selectionRect);
    } else {
        clearSelection();
    }
}

void CropOverlay::keyPressEvent(QKeyEvent *event) {
    if((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) && !clear) {
        emit enterPressed();
    } else if(event->key() == Qt::Key_Escape) {
        emit escPressed();
    } else if(event->matches(QKeySequence::SelectAll)) {
        selectAll();
    } else {
        event->ignore();
    }
}

void CropOverlay::recalculateGeometry() {
    setGeometry(0,0, containerSize().width(), containerSize().height());
}
