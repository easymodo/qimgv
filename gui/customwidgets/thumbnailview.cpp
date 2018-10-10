#include "thumbnailview.h"

ThumbnailView::ThumbnailView(ThumbnailViewOrientation orient, QWidget *parent)
    : QGraphicsView(parent),
      orientation(orient),
      thumbnailSize(130)
{
    setAccessibleName("thumbnailView");
    //setViewport(new QOpenGLWidget());
    this->setMouseTracking(true);
    this->setScene(&scene);
    //scene.setBackgroundBrush();
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); // more buggy than smart
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    this->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    //setAttribute(Qt::WA_OpaquePaintEvent, true);
    //setCacheMode(QGraphicsView::CacheBackground);
    //this->viewport()->setAttribute(Qt::WA_OpaquePaintEvent, true);

    /* scrolling-related things */
    timeLine = new QTimeLine(SCROLL_ANIMATION_SPEED, this);
    timeLine->setEasingCurve(QEasingCurve::OutSine);

    timeLine->setUpdateInterval(SCROLL_UPDATE_RATE);
    scrollTimer.setSingleShot(true);
    scrollTimer.setInterval(40);

    if(orientation == THUMBNAILVIEW_HORIZONTAL) {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollBar = this->verticalScrollBar();
        connect(timeLine, SIGNAL(frameChanged(int)),
                this, SLOT(centerOnX(int)), Qt::UniqueConnection);
    } else {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollBar = this->verticalScrollBar();
        connect(timeLine, SIGNAL(frameChanged(int)),
                this, SLOT(centerOnY(int)), Qt::UniqueConnection);
    }

    scrollBar->setContextMenuPolicy(Qt::NoContextMenu);

    // on scrolling animation finish
    connect(timeLine, SIGNAL(finished()), this, SIGNAL(scrolled()));
    // on manual scrollbar drag
    connect(scrollBar, SIGNAL(sliderMoved(int)), &scrollTimer, SLOT(start()));
    connect(&scrollTimer, SIGNAL(timeout()), this, SIGNAL(scrolled()));
    connect(this, SIGNAL(scrolled()), this, SLOT(loadVisibleThumbnails()));
}

void ThumbnailView::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
    ensureSelectedItemVisible();
    loadVisibleThumbnails();
}

void ThumbnailView::populate(int count) {
    if(count >= 0) {
        for(int i = thumbnails.count() - 1; i >=0; i--) {
            removeItemFromLayout(i);
        }
        qDeleteAll(thumbnails);
        thumbnails.clear();
        for(int i = 0; i < count; i++) {
            ThumbnailWidget *widget = createThumbnailWidget();
            widget->setThumbnailSize(thumbnailSize);
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
// !! calls loadVisibleThumbnails() at the end
void ThumbnailView::insertItem(int index) {
    ThumbnailWidget *widget = createThumbnailWidget();
    thumbnails.insert(index, widget);
    addItemToLayout(widget, index);
    fitSceneToContents();
    loadVisibleThumbnails();
}

void ThumbnailView::removeItem(int index) {
    if(checkRange(index)) {
        removeItemFromLayout(index);
        delete thumbnails.takeAt(index);
        loadVisibleThumbnails();
        fitSceneToContents();
    }
}

void ThumbnailView::setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) {
    if(thumb && thumb->size() == floor(thumbnailSize*qApp->devicePixelRatio()) && checkRange(pos)) {
       // qDebug() ->pos()<< "SET: " << pos;
        thumbnails.at(pos)->setThumbnail(thumb);
        thumbnails.at(pos)->state = LOADED;
    }
}

void ThumbnailView::loadVisibleThumbnails() {
    //loadTimer.stop();
    //qDebug() << "load";
    if(isVisible()) {
        QRectF visibleRect = mapToScene(viewport()->geometry()).boundingRect();
        // grow rectangle to cover nearby offscreen items
        visibleRect.adjust(-OFFSCREEN_PRELOAD_AREA, -OFFSCREEN_PRELOAD_AREA,
                           OFFSCREEN_PRELOAD_AREA, OFFSCREEN_PRELOAD_AREA);
        QList<QGraphicsItem *>visibleItems = scene.items(visibleRect,
                                                   Qt::IntersectsItemShape,
                                                   Qt::AscendingOrder);
        QList<int> loadList;
        for(int i = 0; i < visibleItems.count(); i++) {
            ThumbnailWidget* widget = qgraphicsitem_cast<ThumbnailWidget*>(visibleItems.at(i));
            if(widget->state == EMPTY) {
                loadList.append(thumbnails.indexOf(widget));
            }
        }
        if(loadList.count()) {
            //qDebug() << "requested: " << loadList.count() << " items";
            //qDebug() << loadList;
            emit thumbnailRequested(loadList, static_cast<int>(qApp->devicePixelRatio() * thumbnailSize));
        }
    }
}

void ThumbnailView::centerOnX(int dx) {
    centerOn(dx, viewportCenter.y());
}

void ThumbnailView::centerOnY(int dy) {
    centerOn(viewportCenter.x(), dy);
    //scrollBar->setValue(scrollBar->value()+1);
}

void ThumbnailView::resetViewport() {
    if(timeLine->state() == QTimeLine::Running)
        timeLine->stop();
    scrollBar->setValue(0);
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
    // TODO: if(orient = vertical - then scroll dy etc.
    event->accept();
    viewportCenter = mapToScene(viewport()->rect().center());
    // pixelDelta() with libinput returns non-zero values with mouse wheel
    // so there's no way to distinguish between wheel scroll and touchpad scroll (at least not that i know of)
    // that's why smoothScroll flag workaround
    // NOTE: looks like it's been fixed in current versions. Need testing.
    int pixelDelta = event->pixelDelta().y();
    int angleDelta = event->angleDelta().ry();
    if(pixelDelta != 0)  {
        scrollPrecise(pixelDelta);
    } else {
        scrollSmooth(angleDelta);
    }
}

void ThumbnailView::scrollPrecise(int pixelDelta) {
    // ignore if we reached boundaries
    if( (pixelDelta > 0 && atSceneStart()) || (pixelDelta < 0 && atSceneEnd()) )
        return;
    // pixel scrolling (precise)
    if(orientation == THUMBNAILVIEW_HORIZONTAL) {
        centerOnX(viewportCenter.x() - pixelDelta);
    } else {
        centerOnY(viewportCenter.y() - pixelDelta);
    }
    scrollTimer.start();
}

void ThumbnailView::scrollSmooth(int angleDelta) {
    // ignore if we reached boundaries
    if( (angleDelta > 0 && atSceneStart()) || (angleDelta < 0 && atSceneEnd()) )
        return;

    int center;
    if(orientation == THUMBNAILVIEW_HORIZONTAL) {
        center = viewportCenter.x();
    } else {
        center = viewportCenter.y();
    }
    // smooth scrolling by fixed intervals
    if(timeLine->state() == QTimeLine::Running) {
        timeLine->stop();
        timeLine->setFrameRange(center,
                                timeLine->endFrame() -
                                angleDelta *
                                SCROLL_SPEED_MULTIPLIER * 1.2f);
    } else {
        timeLine->setFrameRange(center,
                                center -
                                angleDelta *
                                SCROLL_SPEED_MULTIPLIER);
    }
    timeLine->start();
}

void ThumbnailView::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        ThumbnailWidget *item = qgraphicsitem_cast<ThumbnailWidget*>(itemAt(event->pos()));
        if(item) {
            emit thumbnailPressed(thumbnails.indexOf(item));
        }
    }
    QGraphicsView::mousePressEvent(event);
}
