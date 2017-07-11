#include "thumbnailview.h"

GraphicsView::GraphicsView(ThumbnailFrame *v)
    : QGraphicsView(), frame(v), forceSmoothScroll(true)
{
    setFocusPolicy(Qt::NoFocus);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // viewport stuff
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // scrolling
    scrollBar = this->horizontalScrollBar();
    scrollBar->setContextMenuPolicy(Qt::NoContextMenu);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    timeLine = new QTimeLine(SCROLL_ANIMATION_SPEED, this);
    timeLine->setEasingCurve(QEasingCurve::OutCubic);
    timeLine->setUpdateInterval(SCROLL_UPDATE_RATE);
    scrollTimer.setSingleShot(true);
    scrollTimer.setInterval(40);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));

    connect(timeLine, SIGNAL(frameChanged(int)),
            this, SLOT(centerOnX(int)), Qt::UniqueConnection);
    // on scrolling animation finish
    connect(timeLine, SIGNAL(finished()), this, SIGNAL(scrolled()));
    // on manual scrollbar drag
    connect(scrollBar, SIGNAL(sliderMoved(int)), &scrollTimer, SLOT(start()));
    connect(&scrollTimer, SIGNAL(timeout()), this, SIGNAL(scrolled()));
    connect(this, SIGNAL(scrolled()), frame, SIGNAL(scrolled()));
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    event->accept();
    viewportCenter = mapToScene(viewport()->rect().center());
    // pixelDelta() with libinput returns non-zero values with mouse wheel
    // so there's no way to distinguish between wheel scroll and touchpad scroll (at least not that i know of)
    // that's why smoothScroll flag workaround
    int pixelDelta = event->pixelDelta().y();
    int angleDelta = event->angleDelta().ry();
    if(!forceSmoothScroll && pixelDelta != 0)  {
        // ignore if we reached boundaries
        if( (pixelDelta > 0 && atSceneStart()) || (pixelDelta < 0 && atSceneEnd()) )
            return;
        // pixel scrolling (precise)
        centerOnX(viewportCenter.x() - event->pixelDelta().y());
        scrollTimer.start();
    } else {
        // also ignore if we reached boundaries
        if( (angleDelta > 0 && atSceneStart()) || (angleDelta < 0 && atSceneEnd()) )
            return;
        // smooth scrolling by fixed intervals
        if(timeLine->state() == QTimeLine::Running) {
            timeLine->stop();
            timeLine->setFrameRange(viewportCenter.x(),
                                    timeLine->endFrame() -
                                    angleDelta *
                                    SCROLL_SPEED_MULTIPLIER*1.2f);
        } else {
            timeLine->setFrameRange(viewportCenter.x(),
                                    viewportCenter.x() -
                                    angleDelta *
                                    SCROLL_SPEED_MULTIPLIER);
        }
        timeLine->start();
    }
}

bool GraphicsView::atSceneStart() {
    if(viewportTransform().dx() == 0)
        return true;
    return false;
}

bool GraphicsView::atSceneEnd() {
    if(viewportTransform().dx() == viewport()->width() - sceneRect().width())
        return true;
    return false;

}

void GraphicsView::centerOnX(int dx) {
    centerOn(dx, viewportCenter.y());
}

void GraphicsView::readSettings() {
    forceSmoothScroll = settings->forceSmoothScroll();
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        ThumbnailLabel *item = qgraphicsitem_cast<ThumbnailLabel*>(itemAt(event->pos()));
        if(item) {
            frame->acceptThumbnailClick(item->labelNum());
        }
        QGraphicsView::mousePressEvent(event);
    }
}

void GraphicsView::resetViewport() {
    if(timeLine->state() == QTimeLine::Running)
        timeLine->stop();
    scrollBar->setValue(0);
}

ThumbnailFrame::ThumbnailFrame(QWidget *parent)
    : QFrame(parent)
{
    setFocusPolicy(Qt::NoFocus);
    graphicsView = new GraphicsView(this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(graphicsView);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
}

GraphicsView *ThumbnailFrame::view() const {
    return graphicsView;
}

void ThumbnailFrame::addItem(QGraphicsItem *item) {
    graphicsView->scene()->addItem(item);
}

void ThumbnailFrame::acceptThumbnailClick(int num) {
    emit thumbnailClicked(num);
}
