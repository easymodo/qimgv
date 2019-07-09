#include "thumbnailview.h"

ThumbnailView::ThumbnailView(ThumbnailViewOrientation orient, QWidget *parent)
    : QGraphicsView(parent),
      orientation(orient),
      blockThumbnailLoading(false),
      mThumbnailSize(120),
      selectedIndex(-1)
{
    setAccessibleName("thumbnailView");
    //scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    this->setMouseTracking(true);
    this->setScene(&scene);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    this->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    //setAttribute(Qt::WA_OpaquePaintEvent, true);
    // breaks panel animation
    //this->viewport()->setAttribute(Qt::WA_OpaquePaintEvent, true);

    /* scrolling-related things */
    scrollTimeLine = new QTimeLine(SCROLL_ANIMATION_SPEED, this);
    scrollTimeLine->setEasingCurve(QEasingCurve::OutSine);
    scrollTimeLine->setUpdateInterval(SCROLL_UPDATE_RATE);

    connect(&loadTimer, SIGNAL(timeout()), this, SLOT(loadVisibleThumbnails()));
    loadTimer.setInterval(static_cast<const int>(LOAD_DELAY));
    loadTimer.setSingleShot(true);

    if(orientation == THUMBNAILVIEW_HORIZONTAL) {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollBar = this->horizontalScrollBar();
        connect(scrollTimeLine, SIGNAL(frameChanged(int)),
                this, SLOT(centerOnX(int)), Qt::UniqueConnection);
    } else {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollBar = this->verticalScrollBar();
        connect(scrollTimeLine, SIGNAL(frameChanged(int)),
                this, SLOT(centerOnY(int)), Qt::UniqueConnection);
    }

    scrollBar->setContextMenuPolicy(Qt::NoContextMenu);

    // on scrolling animation finish
    connect(scrollTimeLine, SIGNAL(finished()), this, SLOT(onSmoothScrollEnd()));
    connect(scrollBar, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

void ThumbnailView::onSmoothScrollEnd() {
    blockThumbnailLoading = false;
    loadVisibleThumbnails();
}

void ThumbnailView::onValueChanged(int value) {
    Q_UNUSED(value)
    loadVisibleThumbnails();
}

void ThumbnailView::setDirectoryPath(QString path) {
    Q_UNUSED(path)
}

void ThumbnailView::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
    ensureSelectedItemVisible();
    loadVisibleThumbnails();
}

// TODO: slow
void ThumbnailView::populate(int count) {
    if(count >= 0) {
        for(int i = thumbnails.count() - 1; i >= 0; i--) {
            removeItemFromLayout(i);
        }
        qDeleteAll(thumbnails);
        thumbnails.clear();
        for(int i = 0; i < count; i++) {
            ThumbnailWidget *widget = createThumbnailWidget();
            widget->setThumbnailSize(mThumbnailSize);
            thumbnails.append(widget);
            addItemToLayout(widget, i);
        }
    }
    updateLayout();
    fitSceneToContents();
    resetViewport();
}

void ThumbnailView::addItem() {
    insertItem(thumbnails.count());
}

// insert at index
void ThumbnailView::insertItem(int index) {
    if(index <= selectedIndex) {
        selectedIndex++;
    }
    ThumbnailWidget *widget = createThumbnailWidget();
    thumbnails.insert(index, widget);
    addItemToLayout(widget, index);
    updateLayout();
    fitSceneToContents();
    loadVisibleThumbnails();
}

void ThumbnailView::removeItem(int index) {
    if(checkRange(index)) {
        if(index < selectedIndex) {
            selectedIndex--;
        } else if(index == selectedIndex) {
            selectedIndex = -1;
        }
        removeItemFromLayout(index);
        delete thumbnails.takeAt(index);
        loadVisibleThumbnails();
        fitSceneToContents();
    }
}

void ThumbnailView::setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) {
    if(thumb && thumb->size() == floor(mThumbnailSize * qApp->devicePixelRatio()) && checkRange(pos)) {
        thumbnails.at(pos)->setThumbnail(thumb);
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
        QList<int> loadList;
        for(int i = 0; i < visibleItems.count(); i++) {
            ThumbnailWidget* widget = qgraphicsitem_cast<ThumbnailWidget*>(visibleItems.at(i));
            if(!widget->isLoaded) {
                loadList.append(thumbnails.indexOf(widget));
            }
        }
        if(loadList.count()) {
            emit thumbnailsRequested(loadList, static_cast<int>(qApp->devicePixelRatio() * mThumbnailSize));
        }
    }
}

void ThumbnailView::loadVisibleThumbnailsDelayed() {
    loadTimer.stop();
    loadTimer.start();
}

void ThumbnailView::centerOnX(int dx) {
    centerOn(dx + 1, viewportCenter.y());
    // trigger repaint immediately
    qApp->processEvents();
}

void ThumbnailView::centerOnY(int dy) {
    centerOn(viewportCenter.x(), dy + 1);
    // trigger repaint immediately
    qApp->processEvents();
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
    if(pos >= 0 && pos < thumbnails.count())
        return true;
    else
        return false;
}

void ThumbnailView::updateLayout() {

}

// fit scene to it's contents size
void ThumbnailView::fitSceneToContents() {
    scene.setSceneRect(scene.itemsBoundingRect());
}

//################### scrolling ######################
void ThumbnailView::wheelEvent(QWheelEvent *event) {
    event->accept();
    viewportCenter = mapToScene(viewport()->rect().center());
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
        else if(angleDelta)
            scrollSmooth(angleDelta);
    }
}

void ThumbnailView::scrollPrecise(int delta) {
    if(scrollTimeLine->state() == QTimeLine::Running)
        scrollTimeLine->stop();
    // ignore if we reached boundaries
    if( (delta > 0 && atSceneStart()) || (delta < 0 && atSceneEnd()) )
        return;
    // pixel scrolling (precise)
    if(orientation == THUMBNAILVIEW_HORIZONTAL) {
        centerOnX(static_cast<int>(viewportCenter.x() - delta));
    } else {
        centerOnY(static_cast<int>(viewportCenter.y() - delta));
    }
    scrollTimer.start();
}

void ThumbnailView::scrollSmooth(int angleDelta) {
    // ignore if we reached boundaries
    if( (angleDelta > 0 && atSceneStart()) || (angleDelta < 0 && atSceneEnd()) )
        return;

    int center;
    if(orientation == THUMBNAILVIEW_HORIZONTAL) {
        center = static_cast<int>(viewportCenter.x());
    } else {
        center = static_cast<int>(viewportCenter.y());
    }
    bool redirect = false;
    int newEndFrame = center - static_cast<int>(angleDelta * SCROLL_SPEED_MULTIPLIER);
    if( (newEndFrame < center && center < scrollTimeLine->endFrame()) || (newEndFrame > center && center > scrollTimeLine->endFrame()) )
    {
        redirect = true;
    }
    if(scrollTimeLine->state() == QTimeLine::Running && !redirect) {
        newEndFrame = scrollTimeLine->endFrame() - static_cast<int>(angleDelta * SCROLL_SPEED_MULTIPLIER * 1.3f);
    }
    scrollTimeLine->stop();
    blockThumbnailLoading = true;
    scrollTimeLine->setFrameRange(center, newEndFrame);
    scrollTimeLine->start();
}

void ThumbnailView::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        ThumbnailWidget *item = dynamic_cast<ThumbnailWidget*>(itemAt(event->pos()));
        if(item) {
            emit thumbnailPressed(thumbnails.indexOf(item));
            return;
        }
    }
    QGraphicsView::mousePressEvent(event);
}
