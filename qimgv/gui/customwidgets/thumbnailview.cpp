#include "thumbnailview.h"

ThumbnailView::ThumbnailView(Qt::Orientation _orientation, QWidget *parent)
    : QGraphicsView(parent),
      blockThumbnailLoading(false),
      mCropThumbnails(false),
      mouseReleaseSelect(false),
      mDrawScrollbarIndicator(true),
      mThumbnailSize(120),
      rangeSelection(false),
      selectMode(ACTIVATE_BY_PRESS),
      lastScrollFrameTime(0),
      scrollTimeLine(nullptr)
{
    setAccessibleName("thumbnailView");
    this->setMouseTracking(true);
    this->setAcceptDrops(false);
    this->setScene(&scene);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setAttribute(Qt::WA_TranslucentBackground, false);
    this->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    this->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, false);

    setOrientation(_orientation);

    lastTouchpadScroll.start();

    connect(&loadTimer, &QTimer::timeout, this, &ThumbnailView::loadVisibleThumbnails);
    loadTimer.setInterval(static_cast<const int>(LOAD_DELAY));
    loadTimer.setSingleShot(true);

    qreal screenMaxRefreshRate = 60;
    for(auto screen : qApp->screens())
        if(screen->refreshRate() > screenMaxRefreshRate)
            screenMaxRefreshRate = screen->refreshRate();
    scrollRefreshRate = 1000 / screenMaxRefreshRate;

    createScrollTimeLine();

    horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    horizontalScrollBar()->installEventFilter(this);
    connect(horizontalScrollBar(), &QScrollBar::valueChanged, [this]() {
        loadVisibleThumbnails();
    });
    verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    verticalScrollBar()->installEventFilter(this);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, [this]() {
        loadVisibleThumbnails();
    });
}

Qt::Orientation ThumbnailView::orientation() {
    return mOrientation;
}

void ThumbnailView::setOrientation(Qt::Orientation _orientation) {
    mOrientation = _orientation;
    if(mOrientation == Qt::Horizontal) {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollBar = this->horizontalScrollBar();
        centerOn = [this](int value) {
            QGraphicsView::centerOn(value + 1, viewportCenter.y());
        };
    } else {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollBar = this->verticalScrollBar();
        centerOn = [this](int value) {
            QGraphicsView::centerOn(viewportCenter.x(), value + 1);
        };
    }
    // todo: layout
}

void ThumbnailView::hideEvent(QHideEvent *event) {
    QGraphicsView::hideEvent(event);
    rangeSelection = false;
}

void ThumbnailView::createScrollTimeLine() {
    if(scrollTimeLine) {
        scrollTimeLine->stop();
        scrollTimeLine->deleteLater();
    }
    scrollTimeLine = new QTimeLine(SCROLL_DURATION, this);
    scrollTimeLine->setEasingCurve(QEasingCurve::OutSine);
    scrollTimeLine->setUpdateInterval(scrollRefreshRate);

    connect(scrollTimeLine, &QTimeLine::frameChanged, [this](int value) {
        scrollFrameTimer.start();
        this->centerOn(value);
        // trigger repaint immediately
        qApp->processEvents();
        lastScrollFrameTime = scrollFrameTimer.elapsed();
        if(scrollTimeLine->state() == QTimeLine::Running && lastScrollFrameTime > scrollRefreshRate) {
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
    if (o == horizontalScrollBar() || o == verticalScrollBar()) {
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

void ThumbnailView::show() {
    QGraphicsView::show();
    focusOnSelection();
    loadVisibleThumbnails();
}

void ThumbnailView::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
    // ensure we are properly resized
    qApp->processEvents();
    updateScrollbarIndicator();
    loadVisibleThumbnails();
}

void ThumbnailView::populate(int newCount) {
    // wait for possible queued layout events before removing items
    qApp->processEvents();

    clearSelection();
    // reset
    lastScrollDirection = SCROLL_FORWARDS;
    // pause updates until the layout is calculated
    // without this you will see scene moving when scrollbar appears
    this->setUpdatesEnabled(false);
    QElapsedTimer t;
    t.start();
    if(newCount >= 0) {
        /* test this later
        // reuse existing items
        auto currentCount = thumbnails.count();
        if(currentCount > newCount) {
            qDebug() << this << "removing";
            for(auto i = currentCount - 1; i >= newCount; i--) {
                //removeItemFromLayout(i); // slow. is this needed?
                delete thumbnails.takeLast();
            }
            // reset existing
            QList<ThumbnailWidget*>::iterator i;
            for(i = thumbnails.begin(); i != thumbnails.end(); ++i)
                (*i)->reset();
        } else if(currentCount <= newCount) {
            // reset existing
            QList<ThumbnailWidget*>::iterator i;
            for(i = thumbnails.begin(); i != thumbnails.end(); ++i)
                (*i)->reset();
            qDebug() << this << "adding";
            for(auto i = currentCount; i < newCount; i++) {
                ThumbnailWidget *widget = createThumbnailWidget();
                widget->setThumbnailSize(mThumbnailSize);
                thumbnails.append(widget);
                addItemToLayout(widget, i);
            }
        }
        */

        if(newCount == thumbnails.count()) {
            QList<ThumbnailWidget*>::iterator i;
            for (i = thumbnails.begin(); i != thumbnails.end(); ++i) {
                (*i)->reset();
            }
        } else {
            removeAll();
            for(int i = 0; i < newCount; i++) {
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
    //qDebug() << "_______POPULATE" << this << t.elapsed();
    // wait for layout before updating
    qApp->processEvents();
    this->setUpdatesEnabled(true);
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
        auto newSelection = mSelection;
        clearSelection();
        removeItemFromLayout(index);
        delete thumbnails.takeAt(index);
        fitSceneToContents();
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

void ThumbnailView::setDragHover(int index) {

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
        QRectF visRect = mapToScene(viewport()->geometry()).boundingRect();
        QRectF offRectBack;
        QRectF offRectFront;
        if(mOrientation == Qt::Horizontal) {
            offRectBack = QRectF(visRect.left() - offscreenPreloadArea, visRect.top(),
                                 visRect.left(), visRect.height());
            offRectFront = QRectF(visRect.right(), visRect.top(),
                                  visRect.right() + offscreenPreloadArea, visRect.height());
        } else {
            offRectBack = QRectF(visRect.left(), visRect.top() - offscreenPreloadArea,
                                 visRect.width(), visRect.top());
            offRectFront = QRectF(visRect.left(), visRect.bottom(),
                                  visRect.width(), visRect.bottom() + offscreenPreloadArea);
        }
        QList<QGraphicsItem *>visibleItems;
        if(lastScrollDirection == SCROLL_FORWARDS)
            visibleItems = scene.items(visRect, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder);
        else
            visibleItems = scene.items(visRect, Qt::IntersectsItemBoundingRect, Qt::DescendingOrder);
        visibleItems.append(scene.items(offRectBack,  Qt::IntersectsItemBoundingRect, Qt::DescendingOrder));
        visibleItems.append(scene.items(offRectFront, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder));
        // select
        QList<int> loadList;
        for(int i = 0; i < visibleItems.count(); i++) {
            ThumbnailWidget* widget = qgraphicsitem_cast<ThumbnailWidget*>(visibleItems.at(i));
            if(widget && !widget->isLoaded) {
                int idx = thumbnails.indexOf(widget);
                if(!loadList.contains(idx))
                    loadList.append(idx);
            }
        }
        // load
        if(loadList.count())
            emit thumbnailsRequested(loadList, static_cast<int>(qApp->devicePixelRatio() * mThumbnailSize), mCropThumbnails, false);
        // unload offscreen
        if(settings->unloadThumbs()) {
            for(int i = 0; i < thumbnails.count(); i++)
                if(!visibleItems.contains(thumbnails.at(i)))
                    thumbnails.at(i)->unsetThumbnail();
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
    if(mOrientation == Qt::Horizontal) {
        if(viewportTransform().dx() == 0.0)
            return true;
    } else {
        if(viewportTransform().dy() == 0.0)
            return true;
    }
    return false;
}

bool ThumbnailView::atSceneEnd() {
    if(mOrientation == Qt::Horizontal) {
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
    if(this->mOrientation == Qt::Vertical) {
        int height = qMax((int)scene.itemsBoundingRect().height(), this->height());
        scene.setSceneRect(QRectF(0,0, this->width(), height));
        center = mapToScene(viewport()->rect().center());
        QGraphicsView::centerOn(0, center.y() + 1);
    } else {
        int width = qMax((int)scene.itemsBoundingRect().width(), this->width());
        scene.setSceneRect(QRectF(0,0, width, this->height()));
        center = mapToScene(viewport()->rect().center());
        QGraphicsView::centerOn(center.x() + 1, 0);
    }
}

//################### scrolling ######################
void ThumbnailView::wheelEvent(QWheelEvent *event) {
    event->accept();
    int pixelDelta = event->pixelDelta().y();
    int angleDelta = event->angleDelta().ry();
    bool isWheel = angleDelta && !(angleDelta % 120) && lastTouchpadScroll.elapsed() > 100;
    if(isWheel) {
        if(!settings->enableSmoothScroll()) {
            if(pixelDelta)
                scrollByItem(pixelDelta);
            else if(angleDelta)
                scrollByItem(angleDelta);
        } else if(angleDelta) { // what about pixelDelta?
            scrollSmooth(angleDelta, SCROLL_MULTIPLIER, SCROLL_ACCELERATION, true);
        }
    } else {
        lastTouchpadScroll.restart();
        if(pixelDelta)
            scrollPrecise(pixelDelta);
        else if(angleDelta)
            scrollPrecise(angleDelta);
    }
}

void ThumbnailView::scrollPrecise(int delta) {
    if(delta < 0)
        lastScrollDirection = SCROLL_FORWARDS;
    else
        lastScrollDirection = SCROLL_BACKWARDS;
    viewportCenter = mapToScene(viewport()->rect().center());
    if(scrollTimeLine->state() == QTimeLine::Running) {
        scrollTimeLine->stop();
        blockThumbnailLoading = false;
    }
    // ignore if we reached boundaries
    if( (delta > 0 && atSceneStart()) || (delta < 0 && atSceneEnd()) )
        return;
    // pixel scrolling (precise)
    if(mOrientation == Qt::Horizontal)
        centerOn(static_cast<int>(viewportCenter.x() - delta));
    else
        centerOn(static_cast<int>(viewportCenter.y() - delta));
}

// windows explorer-like behavior
// scrolls exactly by item width / height
void ThumbnailView::scrollByItem(int delta) {
    // do not scroll less than a certain value in px, to avoid feeling unresponsive
    int minScroll = qMin(thumbnailSize() / 2, 100);
    // grab fully visible thumbs
    QRectF visRect = mapToScene(viewport()->geometry()).boundingRect().adjusted(-minScroll,-minScroll,minScroll,minScroll);
    QList<QGraphicsItem *> visibleItems;
    visibleItems = scene.items(visRect, Qt::ContainsItemBoundingRect, Qt::AscendingOrder);
    if(thumbnails.isEmpty() || visibleItems.isEmpty())
        return;
    int target = 0;
    // select scroll target
    if(delta > 0) { // up / left
        ThumbnailWidget* widget = qgraphicsitem_cast<ThumbnailWidget*>(visibleItems.first());
        if(!widget)
            return;
        target = thumbnails.indexOf(widget) - 1;
    } else { // down / right
        ThumbnailWidget* widget = qgraphicsitem_cast<ThumbnailWidget*>(visibleItems.last());
        if(!widget)
            return;
        target = thumbnails.indexOf(widget) + 1;
    }
    scrollToItem(target);
}

void ThumbnailView::scrollToItem(int index) {
    if(!checkRange(index))
        return;
    ThumbnailWidget *item = thumbnails.at(index);
    QRectF sceneRect = mapToScene(viewport()->rect()).boundingRect();
    QRectF itemRect = item->mapRectToScene(item->rect());
    bool visible = sceneRect.contains(itemRect);
    if(!visible) {
        int delta = 0;
        if(mOrientation == Qt::Vertical) {
            if(itemRect.top() >= sceneRect.top()) // UP
                delta = sceneRect.bottom() - itemRect.bottom();
            else // DOWN
                delta = sceneRect.top() - itemRect.top();
        } else {
            if(itemRect.left() >= sceneRect.left()) // LEFT
                delta = sceneRect.right() - itemRect.right();
            else // RIGHT
                delta = sceneRect.left() - itemRect.left();
        }
        if(settings->enableSmoothScroll())
            scrollSmooth(delta);
        else
            scrollPrecise(delta);
    }
}

void ThumbnailView::scrollSmooth(int delta, qreal multiplier, qreal acceleration, bool additive) {
    if(delta < 0)
        lastScrollDirection = SCROLL_FORWARDS;
    else
        lastScrollDirection = SCROLL_BACKWARDS;
    viewportCenter = mapToScene(viewport()->rect().center());
    // ignore if we reached boundaries
    if( (delta > 0 && atSceneStart()) || (delta < 0 && atSceneEnd()) ) {
        return;
    }
    int center;
    if(mOrientation == Qt::Horizontal)
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
        if(scrollTimeLine->currentTime() < SCROLL_ACCELERATION_THRESHOLD)
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
        scrollTimeLine->setDuration(SCROLL_DURATION / SCROLL_ACCELERATION);
    else
        scrollTimeLine->setDuration(SCROLL_DURATION);
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
    mouseReleaseSelect = false;
    dragStartPos = QPoint(0,0);
    ThumbnailWidget *item = dynamic_cast<ThumbnailWidget*>(itemAt(event->pos()));
    if(item) {
        int index = thumbnails.indexOf(item);
        if(event->button() == Qt::LeftButton) {
            if(event->modifiers() & Qt::ControlModifier) {
                if(!selection().contains(index))
                    select(selection() << index);
                else
                    deselect(index);
            } else if(event->modifiers() & Qt::ShiftModifier) {
                addSelectionRange(index);
            } else if (selection().count() <= 1) {
                if(selectMode == ACTIVATE_BY_PRESS) {
                    emit itemActivated(index);
                    return;
                } else {
                    select(index);
                }
            } else {
                mouseReleaseSelect = true;
            }
            dragStartPos = event->pos();
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
        auto *item = dynamic_cast<ThumbnailWidget*>(itemAt(dragStartPos));
        if(item && selection().contains(thumbnails.indexOf(item)))
            emit draggedOut();
    }
}

void ThumbnailView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
    if(mouseReleaseSelect && QLineF(dragStartPos, event->pos()).length() < 40) {
        ThumbnailWidget *item = dynamic_cast<ThumbnailWidget*>(itemAt(event->pos()));
        if(item) {
            int index = thumbnails.indexOf(item);
            select(index);
        }
    }
}

void ThumbnailView::mouseDoubleClickEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        ThumbnailWidget *item = dynamic_cast<ThumbnailWidget*>(itemAt(event->pos()));
        if(item) {
            emit itemActivated(thumbnails.indexOf(item));
            return;
        }
    }
    event->ignore();
}

void ThumbnailView::focusOutEvent(QFocusEvent *event) {
    QGraphicsView::focusOutEvent(event);
    rangeSelection = false;
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
