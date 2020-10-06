#include "thumbnailview.h"

ThumbnailView::ThumbnailView(ThumbnailViewOrientation orient, QWidget *parent)
    : QGraphicsView(parent),
      orientation(orient),
      blockThumbnailLoading(false),
      mCropThumbnails(false),
      mDrawScrollbarIndicator(true),
      mThumbnailSize(120),
      rangeSelection(false),
      selectMode(SELECT_BY_PRESS),
      lastScrollFrameTime(0),
      scrollTimeLine(nullptr)
{
    setAccessibleName("thumbnailView");
    this->setMouseTracking(true);
    this->setAcceptDrops(false);
    this->setScene(&scene);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    this->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    this->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, false);


    connect(&loadTimer, &QTimer::timeout, this, &ThumbnailView::loadVisibleThumbnails);
    loadTimer.setInterval(static_cast<const int>(LOAD_DELAY));
    loadTimer.setSingleShot(true);

    if(orientation == THUMBNAILVIEW_HORIZONTAL) {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollBar = this->horizontalScrollBar();
        centerOn = [this](int value) {
            QGraphicsView::centerOn(value + 1, viewportCenter.y());
            // trigger repaint immediately
            qApp->processEvents();
        };
    } else {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollBar = this->verticalScrollBar();
        centerOn = [this](int value) {
            QGraphicsView::centerOn(viewportCenter.x(), value + 1);
            // trigger repaint immediately
            qApp->processEvents();
        };
    }
    createScrollTimeLine();

    scrollBar->setContextMenuPolicy(Qt::NoContextMenu);
    scrollBar->installEventFilter(this);

    connect(scrollBar, &QScrollBar::valueChanged, [this]() {
        loadVisibleThumbnails();
    });
}

void ThumbnailView::createScrollTimeLine() {
    if(scrollTimeLine) {
        scrollTimeLine->stop();
        scrollTimeLine->deleteLater();
    }
    /* scrolling-related things */
    scrollTimeLine = new QTimeLine(SCROLL_SPEED, this);
    scrollTimeLine->setEasingCurve(QEasingCurve::OutSine);
    scrollTimeLine->setUpdateInterval(SCROLL_UPDATE_RATE);

    connect(scrollTimeLine, &QTimeLine::frameChanged, [this](int value) {
        scrollFrameTimer.start();
        this->centerOn(value);
        lastScrollFrameTime = scrollFrameTimer.elapsed();
        if(scrollTimeLine->state() == QTimeLine::Running && lastScrollFrameTime > SCROLL_UPDATE_RATE) {
            scrollTimeLine->setPaused(true);
            int newTime = qMin(scrollTimeLine->duration(), scrollTimeLine->currentTime() + lastScrollFrameTime);
            scrollTimeLine->setCurrentTime(newTime);
            scrollTimeLine->setPaused(false);
        }
    });

    connect(scrollTimeLine, &QTimeLine::finished, [this]() {
        blockThumbnailLoading = false;
        loadVisibleThumbnails();
    });
}

bool ThumbnailView::eventFilter(QObject *o, QEvent *ev) {
    if (o == scrollBar) {
        if(ev->type() == QEvent::Wheel) {
            this->wheelEvent(dynamic_cast<QWheelEvent*>(ev));
            return true;
        } else if(ev->type() == QEvent::Paint && mDrawScrollbarIndicator) {
            QPainter p(scrollBar);
            p.setOpacity(0.3f);
            p.fillRect(indicator, QBrush(Qt::gray));
            p.setOpacity(1.0f);
            return false;
        }

    }
    return QObject::eventFilter(o, ev);
}

void ThumbnailView::setDirectoryPath(QString path) {
    Q_UNUSED(path)
}

void ThumbnailView::select(QList<int> indices) {
    for(auto i : mSelection)
        thumbnails.at(i)->setHighlighted(false);
    QList<int>::iterator it = indices.begin();
    while(it != indices.end()) {
        // sanity check
        if(*it < 0 || *it >= itemCount()) {
            it = indices.erase(it);
        } else {
            thumbnails.at(*it)->setHighlighted(true);
            ++it;
        }
    }
    mSelection = indices;
    updateScrollbarIndicator();
}

void ThumbnailView::select(int index) {
    // fallback
    if(!checkRange(index))
        index = 0;
    this->select(QList<int>() << index);
}

void ThumbnailView::deselect(int index) {
    if(!checkRange(index))
            return;
    if(mSelection.count() > 1) {
        mSelection.removeAll(index);
        thumbnails.at(index)->setHighlighted(false);
    }
}

void ThumbnailView::addSelectionRange(int indexTo) {
    if(!rangeSelectionSnapshot.count() || !selection().count())
        return;
    auto list = rangeSelectionSnapshot;
    if(indexTo > rangeSelectionSnapshot.last()) {
        for(int i = rangeSelectionSnapshot.last() + 1; i <= indexTo; i++) {
            if(list.contains(i))
                list.removeAll(i);
            list << i;
        }
    } else {
        for(int i = rangeSelectionSnapshot.last() - 1; i >= indexTo; i--) {
            if(list.contains(i))
                list.removeAll(i);
            list << i;
        }
    }
    select(list);
}

QList<int> ThumbnailView::selection() {
    return mSelection;
}

void ThumbnailView::clearSelection() {
    for(auto i : mSelection)
        thumbnails.at(i)->setHighlighted(false);
    mSelection.clear();
}

int ThumbnailView::lastSelected() {
    if(!selection().count())
        return -1;
    else
        return selection().last();
}

int ThumbnailView::itemCount() {
    return thumbnails.count();
}

void ThumbnailView::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
    // ensure we are properly resized
    qApp->processEvents();
    updateScrollbarIndicator();
    ensureSelectedItemVisible();
    loadVisibleThumbnails();
}

void ThumbnailView::populate(int count) {
    clearSelection();
    if(count >= 0) {
        // reuse existing items
        if(count == thumbnails.count()) {
            QList<ThumbnailWidget*>::iterator i;
            for (i = thumbnails.begin(); i != thumbnails.end(); ++i) {
                (*i)->reset();
            }
        } else {
            removeAll();
            for(int i = 0; i < count; i++) {
                ThumbnailWidget *widget = createThumbnailWidget();
                widget->setThumbnailSize(mThumbnailSize);
                thumbnails.append(widget);
                addItemToLayout(widget, i);
            }
        }
    }
    updateLayout();
    fitSceneToContents();
    resetViewport();
    loadVisibleThumbnails();
}

void ThumbnailView::addItem() {
    insertItem(thumbnails.count());
}

// insert at index
void ThumbnailView::insertItem(int index) {
    ThumbnailWidget *widget = createThumbnailWidget();
    thumbnails.insert(index, widget);
    addItemToLayout(widget, index);
    updateLayout();
    fitSceneToContents();

    auto newSelection = mSelection;
    for(int i=0; i < newSelection.count(); i++) {
        if(index <= newSelection[i])
            newSelection[i]++;
    }
    select(newSelection);

    updateScrollbarIndicator();
    loadVisibleThumbnails();
}

void ThumbnailView::removeItem(int index) {
    if(checkRange(index)) {
        removeItemFromLayout(index);
        delete thumbnails.takeAt(index);
        fitSceneToContents();
        mSelection.removeAll(index);
        auto newSelection = mSelection;
        newSelection.removeAll(index);
        for(int i=0; i < newSelection.count(); i++) {
            if(newSelection[i] >= index)
                newSelection[i]--;
        }
        if(!newSelection.count() && itemCount())
            newSelection << ((index >= itemCount()) ? itemCount() - 1 : index);
        select(newSelection);
        updateScrollbarIndicator();
        loadVisibleThumbnails();
    }
}

void ThumbnailView::reloadItem(int index) {
    if(!checkRange(index))
        return;
    auto thumb = thumbnails.at(index);
    if(thumb->isLoaded)
        thumb->unsetThumbnail();
    emit thumbnailsRequested(QList<int>() << index, static_cast<int>(qApp->devicePixelRatio() * mThumbnailSize), mCropThumbnails, true);
}

void ThumbnailView::setCropThumbnails(bool mode) {
    if(mode != mCropThumbnails) {
        unloadAllThumbnails();
        mCropThumbnails = mode;
        loadVisibleThumbnails();
    }
}

void ThumbnailView::setDrawScrollbarIndicator(bool mode) {
    mDrawScrollbarIndicator = mode;
}

void ThumbnailView::setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) {
    if(thumb && thumb->size() == floor(mThumbnailSize * qApp->devicePixelRatio()) && checkRange(pos)) {
        thumbnails.at(pos)->setThumbnail(thumb);
    }
}

void ThumbnailView::unloadAllThumbnails() {
    for(int i = 0; i < thumbnails.count(); i++) {
        thumbnails.at(i)->unsetThumbnail();
    }
}

void ThumbnailView::loadVisibleThumbnails() {
    loadTimer.stop();
    if(isVisible() && !blockThumbnailLoading) {
        QRectF visibleRect = mapToScene(viewport()->geometry()).boundingRect();
        // grow rectangle to cover nearby offscreen items
        visibleRect.adjust(-offscreenPreloadArea, -offscreenPreloadArea,
                           offscreenPreloadArea, offscreenPreloadArea);
        QList<QGraphicsItem *>visibleItems = scene.items(visibleRect,
                                                         Qt::IntersectsItemShape,
                                                         Qt::AscendingOrder);
        // load new previews
        QList<int> loadList;
        for(int i = 0; i < visibleItems.count(); i++) {
            ThumbnailWidget* widget = qgraphicsitem_cast<ThumbnailWidget*>(visibleItems.at(i));
            if(widget && !widget->isLoaded)
                loadList.append(thumbnails.indexOf(widget));
        }
        if(loadList.count()) {
            emit thumbnailsRequested(loadList, static_cast<int>(qApp->devicePixelRatio() * mThumbnailSize), mCropThumbnails, false);
        }
        // unload offscreen
        for(int i = 0; i < thumbnails.count(); i++) {
            if(!visibleItems.contains(thumbnails.at(i))) {
                thumbnails.at(i)->unsetThumbnail();
            }
        }
    }
}

void ThumbnailView::loadVisibleThumbnailsDelayed() {
    loadTimer.stop();
    loadTimer.start();
}

void ThumbnailView::resetViewport() {
    if(scrollTimeLine->state() == QTimeLine::Running)
        scrollTimeLine->stop();
    scrollBar->setValue(0);
}

int ThumbnailView::thumbnailSize() {
    return mThumbnailSize;
}

bool ThumbnailView::atSceneStart() {
    if(orientation == THUMBNAILVIEW_HORIZONTAL) {
        if(viewportTransform().dx() == 0.0)
            return true;
    } else {
        if(viewportTransform().dy() == 0.0)
            return true;
    }
    return false;
}

bool ThumbnailView::atSceneEnd() {
    if(orientation == THUMBNAILVIEW_HORIZONTAL) {
        if(viewportTransform().dx() == viewport()->width() - sceneRect().width())
            return true;
    } else {
        if(viewportTransform().dy() == viewport()->height() - sceneRect().height())
            return true;
    }
    return false;
}

bool ThumbnailView::checkRange(int pos) {
    return pos >= 0 && pos < thumbnails.count();
}

void ThumbnailView::updateLayout() {

}

// fit scene to it's contents size
void ThumbnailView::fitSceneToContents() {
    QPointF center;
    if(this->orientation == THUMBNAILVIEW_VERTICAL) {
        int height = qMax((int)scene.itemsBoundingRect().height(), this->height());
        scene.setSceneRect(QRectF(0,0, this->width(), height));
        center = mapToScene(viewport()->rect().center());
        QGraphicsView::centerOn(0, center.y() + 1);
    } else {
        int width = qMax((int)scene.itemsBoundingRect().width(), this->width());
        scene.setSceneRect(QRectF(0,0, width, this->height()));
        center = mapToScene(viewport()->rect().center());
        QGraphicsView::centerOn(center.x(), 0);
    }
}

//################### scrolling ######################
void ThumbnailView::wheelEvent(QWheelEvent *event) {
    event->accept();
    // alright, i officially gave up on fixing libinput scrolling
    // let's just hope it gets done in Qt while I am still alive
    int pixelDelta = event->pixelDelta().y();
    int angleDelta = event->angleDelta().ry();
    if(!settings->enableSmoothScroll()) {
        if(pixelDelta)
            scrollPrecise(pixelDelta);
        else if(angleDelta)
            scrollPrecise(angleDelta);
    } else {
        if(pixelDelta)
            scrollPrecise(pixelDelta);
        else if(abs(angleDelta) < SMOOTH_SCROLL_THRESHOLD)
            scrollPrecise(angleDelta);
        else if(angleDelta)
            scrollSmooth(angleDelta, SCROLL_MULTIPLIER, SCROLL_ACCELERATION, true);
    }
}

void ThumbnailView::scrollPrecise(int delta) {
    viewportCenter = mapToScene(viewport()->rect().center());
    if(scrollTimeLine->state() == QTimeLine::Running) {
        scrollTimeLine->stop();
        blockThumbnailLoading = false;
    }
    // ignore if we reached boundaries
    if( (delta > 0 && atSceneStart()) || (delta < 0 && atSceneEnd()) )
        return;
    // pixel scrolling (precise)
    if(orientation == THUMBNAILVIEW_HORIZONTAL)
        centerOn(static_cast<int>(viewportCenter.x() - delta));
    else
        centerOn(static_cast<int>(viewportCenter.y() - delta));
}

void ThumbnailView::scrollSmooth(int delta, qreal multiplier, qreal acceleration, bool additive) {
    viewportCenter = mapToScene(viewport()->rect().center());
    // ignore if we reached boundaries
    if( (delta > 0 && atSceneStart()) || (delta < 0 && atSceneEnd()) ) {
        return;
    }
    int center;
    if(orientation == THUMBNAILVIEW_HORIZONTAL)
        center = static_cast<int>(viewportCenter.x());
    else
        center = static_cast<int>(viewportCenter.y());
    bool redirect = false, accelerate = false;
    int newEndFrame = center - static_cast<int>(delta * multiplier);
    if( (newEndFrame < center && center < scrollTimeLine->endFrame()) ||
        (newEndFrame > center && center > scrollTimeLine->endFrame()) )
    {
        redirect = true;
    }
    if(scrollTimeLine->state() == QTimeLine::Running || scrollTimeLine->state() == QTimeLine::Paused) {
        int oldEndFrame = scrollTimeLine->endFrame();
        accelerate = true;
        // QTimeLine has this weird issue when it is already finished (at the last frame)
        // but is stuck in the running state. So we just create a new one.
        if(oldEndFrame == center)
            createScrollTimeLine();
        if(!redirect && additive)
            newEndFrame = oldEndFrame - static_cast<int>(delta * multiplier * acceleration);
    }
    scrollTimeLine->stop();
    if(accelerate)
        scrollTimeLine->setDuration(SCROLL_SPEED / SCROLL_SPEED_ACCELERATION);
    else
        scrollTimeLine->setDuration(SCROLL_SPEED);
    //blockThumbnailLoading = true;
    scrollTimeLine->setFrameRange(center, newEndFrame);
    scrollTimeLine->start();
}

void ThumbnailView::scrollSmooth(int delta, qreal multiplier, qreal acceleration) {
    scrollSmooth(delta, multiplier, acceleration, false);
}

void ThumbnailView::scrollSmooth(int angleDelta) {
    scrollSmooth(angleDelta, 1.0, 1.0, false);
}

void ThumbnailView::mousePressEvent(QMouseEvent *event) {
    dragStartPos = QPointF(0,0);
    ThumbnailWidget *item = dynamic_cast<ThumbnailWidget*>(itemAt(event->pos()));
    if(item) {
        int index = thumbnails.indexOf(item);
        if(event->button() == Qt::LeftButton) {
            if(selectMode == SELECT_BY_PRESS) {
                emit itemActivated(index);
                return;
            } else {
                if(event->modifiers() & Qt::ControlModifier) {
                    if(!selection().contains(index))
                        select(selection() << index);
                    else
                        deselect(index);
                } else if(event->modifiers() & Qt::ShiftModifier) {
                    addSelectionRange(index);
                } else {
                    select(index);
                }
                dragStartPos = event->pos();
            }
        } else if(event->button() == Qt::RightButton) { // todo: context menu maybe?
            select(index);
            return;
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void ThumbnailView::mouseMoveEvent(QMouseEvent *event) {
    QGraphicsView::mouseMoveEvent(event);
    if(event->buttons() != Qt::LeftButton || !selection().count())
        return;
    if(QLineF(dragStartPos, event->pos()).length() >= 40) {
        emit draggedOut();
    }
}

void ThumbnailView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
}

void ThumbnailView::mouseDoubleClickEvent(QMouseEvent *event) {
    if(selectMode == SELECT_BY_DOUBLECLICK) {
        if(event->button() == Qt::LeftButton) {
            ThumbnailWidget *item = dynamic_cast<ThumbnailWidget*>(itemAt(event->pos()));
            if(item) {
                emit itemActivated(thumbnails.indexOf(item));
                return;
            }
        }
    }
    event->ignore();
}

void ThumbnailView::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Shift)
        rangeSelectionSnapshot = selection();
    if(event->modifiers() & Qt::ShiftModifier)
        rangeSelection = true;
}

void ThumbnailView::keyReleaseEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Shift)
        rangeSelection = false;
}

void ThumbnailView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    updateScrollbarIndicator();
}

void ThumbnailView::setSelectMode(ThumbnailSelectMode mode) {
    selectMode = mode;
}
