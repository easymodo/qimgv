#include "cropoverlay.h"

// TODO: this is pretty old. Clean up

CropOverlay::CropOverlay(OverlayContainerWidget *parent) : OverlayWidget(parent),
    viewer(parent),
    startPos(QPoint(0, 0)),
    endPos(QPoint(0, 0)),
    imageDrawRect(QRect(0, 0, 0, 0)),
    selectionRect(QRect(0, 0, 0, 0)),
    clear(true),
    moving(false),
    forceAspectRatio(false),
    scale(1.0f),
    drawBuffer(nullptr),
    cursorAction(NO_DRAG)
{
    setMouseTracking(true);
    dpr = devicePixelRatioF();
    handleSize = static_cast<int>(8 * dpr);
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

    //tmp for testing
    ar.setX(16);
    ar.setY(9);

    hide();
}

void CropOverlay::prepareDrawElements() {
    for(int i = 0; i < 8; i++) {
        handles[i] = new QRectF(0, 0, handleSize * 2, handleSize * 2);
    }
}

void CropOverlay::setImageRealSize(QSize sz) {
    imageRect.setSize(sz);
    selectAll();
}

void CropOverlay::setImageDrawRect(QRect imageDrawRect) {
    if(this->imageDrawRect != imageDrawRect) {
        this->imageDrawRect = imageDrawRect;
    }
}

void CropOverlay::setImageScale(float scale) {
    this->scale = scale;
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

void CropOverlay::setForceAspectRatio(bool mode) {
    forceAspectRatio = mode;
}

void CropOverlay::setAspectRatio(QPointF ratio) {
    if(ratio.x() == 0.0 || ratio.y() == 0.0)
        return;
    ar = ratio;
    setForceAspectRatio(true);
    // force resize selection area
    if(forceAspectRatio && !clear) {
        resizeSelection(QPoint(0,0));
        update();
    }
}

void CropOverlay::show() {
    if(!drawBuffer) {
        drawBuffer = new QImage(size() * dpr, QImage::Format_ARGB32_Premultiplied);
        drawBuffer->setDevicePixelRatio(dpr);
    }
    QWidget::show();
    selectAll();
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
        drawBuffer = nullptr;
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
        painter.drawRect(QRect(imageDrawRect.topLeft() / dpr, imageDrawRect.size() / dpr));
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
QRect CropOverlay::placeInside(QRect inner, QRect outer) {
    if(inner.width() > outer.width()) {
        inner.setLeft(outer.left());
        inner.setRight(outer.right());
    } else {
        if(inner.left() < outer.left())
            inner.moveLeft(outer.left());
        if(inner.right() > outer.right())
            inner.moveRight(outer.right());
    }
    if(inner.height() > outer.height()) {
        inner.setTop(outer.top());
        inner.setBottom(outer.bottom());
    } else {
        if(inner.top() < outer.top())
            inner.moveTop(outer.top());
        if(inner.bottom() > outer.bottom())
            inner.moveBottom(outer.bottom());
    }
    return inner;
}

CursorAction CropOverlay::hoverTarget(QPoint pos) {
    CursorAction target;
    if(handles[0]->contains(pos)) {
        target = DRAG_TOPLEFT;
    } else if(handles[1]->contains(pos)) {
        target = DRAG_TOPRIGHT;
    } else if(handles[2]->contains(pos)) {
        target = DRAG_BOTTOMLEFT;
    } else if(handles[3]->contains(pos)) {
        target = DRAG_BOTTOMRIGHT;
    } else if(handles[4]->contains(pos)) {
        target = DRAG_LEFT;
    } else if(handles[5]->contains(pos)) {
        target = DRAG_RIGHT;
    } else if(handles[6]->contains(pos)) {
        target = DRAG_TOP;
    } else if(handles[7]->contains(pos)) {
        target = DRAG_BOTTOM;
    } else if(selectionDrawRect.contains(pos)) {
        target = DRAG_MOVE;
    } else {
        target = NO_DRAG;
    }
    return target;
}

void CropOverlay::setCursorAction(CursorAction action) {
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

void CropOverlay::setResizeAnchor(CursorAction action) {
    switch(action) {
        case DRAG_TOPLEFT: resizeAnchor = selectionRect.bottomRight(); break;
        case DRAG_TOPRIGHT: resizeAnchor = selectionRect.bottomLeft(); break;
        case DRAG_BOTTOMLEFT: resizeAnchor = selectionRect.topRight(); break;
        case DRAG_BOTTOMRIGHT: resizeAnchor = selectionRect.topLeft(); break;
        case DRAG_LEFT: resizeAnchor = selectionRect.topRight(); break;
        case DRAG_RIGHT: resizeAnchor = selectionRect.bottomLeft(); break;
        case DRAG_TOP: resizeAnchor = selectionRect.bottomLeft(); break;
        case DRAG_BOTTOM: resizeAnchor = selectionRect.topLeft(); break;
        default: break;
    }
}

// TODO: flip selection rectangle
void CropOverlay::resizeSelection(QPoint delta) {
    if(forceAspectRatio)
        resizeSelectionAR2(delta);
    else
        resizeSelectionFree(delta);
    updateSelectionDrawRect();
    updateHandlePositions();
}

// TODO: split this up
void CropOverlay::resizeSelectionAR2(QPoint delta) {
    QSizeF newSz(selectionRect.size());
    QSizeF maxSz;
    /* ~JustQtThings~
     * int QRect::bottom() const
     *   Note that for historical reasons this function returns top() + height() - 1;
     *   use y() + height() to retrieve the true y-coordinate.
     * same for QRect::right()
     */
    switch(cursorAction) {
        case DRAG_TOPLEFT:
            // get max selection size
            maxSz.setWidth(selectionRect.right() + 1);
            maxSz.setHeight(selectionRect.bottom() + 1);
            // apply ar to current size
            newSz.setHeight(newSz.width() / ar.x() * ar.y());
            // scale to fit
            newSz.scale(qMin(newSz.width() - delta.x(), maxSz.width()),
                        maxSz.height(),
                        Qt::KeepAspectRatio);
            // apply
            selectionRect.setSize(newSz.toSize());
            // move the corner so it stays in original place
            selectionRect.moveBottomRight(resizeAnchor);
            break;
        case DRAG_TOPRIGHT:
            maxSz.setWidth(imageRect.width() - selectionRect.left());
            maxSz.setHeight(selectionRect.bottom() + 1);
            newSz.setHeight(newSz.width() / ar.x() * ar.y());
            newSz.scale(qMin(newSz.width() + delta.x(), maxSz.width()),
                        maxSz.height(),
                        Qt::KeepAspectRatio);
            selectionRect.setSize(newSz.toSize());
            selectionRect.moveBottomLeft(resizeAnchor);
            break;
        case DRAG_LEFT:
        case DRAG_BOTTOMLEFT:
            maxSz.setWidth(selectionRect.right() + 1);
            maxSz.setHeight(imageRect.height() - selectionRect.top());
            newSz.setHeight(newSz.width() / ar.x() * ar.y());
            newSz.scale(qMin(newSz.width() - delta.x(), maxSz.width()),
                        maxSz.height(),
                        Qt::KeepAspectRatio);
            selectionRect.setSize(newSz.toSize());
            selectionRect.moveTopRight(resizeAnchor);
            break;
        case DRAG_TOP:
            maxSz.setWidth(imageRect.width() - selectionRect.left());
            maxSz.setHeight(selectionRect.bottom() + 1);
            newSz.setHeight(newSz.width() / ar.x() * ar.y());
            newSz.scale(maxSz.width(),
                        qMin(newSz.height() - delta.y(), maxSz.height()),
                        Qt::KeepAspectRatio);
            selectionRect.setSize(newSz.toSize());
            selectionRect.moveBottomLeft(resizeAnchor);
            break;
        case DRAG_BOTTOM:
            maxSz.setWidth(imageRect.width() - selectionRect.left());
            maxSz.setHeight(imageRect.height() - selectionRect.top());
            newSz.setHeight(newSz.width() / ar.x() * ar.y());
            newSz.scale(maxSz.width(),
                        qMin(newSz.height() + delta.y(), maxSz.height()),
                        Qt::KeepAspectRatio);
            selectionRect.setSize(newSz.toSize());
            break;
        case DRAG_RIGHT:
        case DRAG_BOTTOMRIGHT:
        default:
            maxSz.setWidth(imageRect.width() - selectionRect.left());
            maxSz.setHeight(imageRect.height() - selectionRect.top());
            newSz.setHeight(newSz.width() / ar.x() * ar.y());
            newSz.scale(qMin(newSz.width() + delta.x(), maxSz.width()),
                        maxSz.height(),
                        Qt::KeepAspectRatio);
            selectionRect.setSize(newSz.toSize());
        break;
    }
}

void CropOverlay::resizeSelectionFree(QPoint delta) {
    // RESIZE_FREE
    switch(cursorAction) {
        case DRAG_TOPLEFT:
            selectionRect.setTopLeft(selectionRect.topLeft() + delta);
            break;
        case DRAG_TOPRIGHT:
            selectionRect.setTopRight(selectionRect.topRight() + delta);
            break;
        case DRAG_BOTTOMLEFT:
            selectionRect.setBottomLeft(selectionRect.bottomLeft() + delta);
            break;
        case DRAG_BOTTOMRIGHT:
            selectionRect.setBottomRight(selectionRect.bottomRight() + delta);
            break;
        case DRAG_LEFT:
            selectionRect.setLeft(selectionRect.left() + delta.x());
            break;
        case DRAG_RIGHT:
            selectionRect.setRight(selectionRect.right() + delta.x());
            break;
        case DRAG_TOP:
            selectionRect.setTop(selectionRect.top() + delta.y());
            break;
        case DRAG_BOTTOM:
            selectionRect.setBottom(selectionRect.bottom() + delta.y());
            break;
        default:
            break;
    }

    // crop selection so it's bounded by imageRect
    selectionRect = selectionRect.intersected(imageRect);
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
        cursorAction = hoverTarget(event->pos() * dpr);
        setCursorAction(cursorAction);
        setResizeAnchor(cursorAction);
        moveStartPos = event->pos();
    }
}

void CropOverlay::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton && !clear) {
        QPoint delta = (event->pos() - moveStartPos) * dpr;
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
        } else if(cursorAction == NO_DRAG) {
            // ignore. we should always have some selection rectangle

            /*
            endPos = event->pos();
            //build selection rectangle
            QPoint tl, br;
            startPos.x() >= endPos.x() ? tl.setX(endPos.x()) : tl.setX(startPos.x());
            startPos.y() >= endPos.y() ? tl.setY(endPos.y()) : tl.setY(startPos.y());

            startPos.x() <= endPos.x() ? br.setX(endPos.x()) : br.setX(startPos.x());
            startPos.y() <= endPos.y() ? br.setY(endPos.y()) : br.setY(startPos.y());

            selectionRect.setTopLeft(mapPointToImage(tl * dpr));
            selectionRect.setBottomRight(mapPointToImage(br * dpr));

            updateSelectionDrawRect();
            updateHandlePositions();
            update();
            */
        } else { // resizing selection
            resizeSelection(delta / scale);
            moveStartPos = event->pos();
            update();
        }
        emit selectionChanged(selectionRect);
    } else {
        if(clear)
            setCursor(QCursor(Qt::ArrowCursor));
        else
            setCursorAction(hoverTarget(event->pos() * dpr));
    }
}

void CropOverlay::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    cursorAction = NO_DRAG;
    setCursorAction(hoverTarget(event->pos() * dpr));
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

// TODO: use aspect ratio
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
        selectAll();
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
    // recreate buffer with a new size
    if(drawBuffer) {
        delete drawBuffer;
        drawBuffer = new QImage(size() * dpr, QImage::Format_ARGB32_Premultiplied);
        drawBuffer->setDevicePixelRatio(dpr);
    }
}
