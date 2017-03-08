#include "thumbnailview.h"

GraphicsView::GraphicsView(ThumbnailFrame *v)
    : QGraphicsView(), frame(v), forceSmoothScroll(true)
{
    setFocusPolicy(Qt::NoFocus);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    scrollBar = this->horizontalScrollBar();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // TODO: fix vertical alignment
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    timeLine = new QTimeLine(SCROLL_ANIMATION_SPEED, this);
    //timeLine->setCurveShape(QTimeLine::EaseInCurve);
    timeLine->setEasingCurve(QEasingCurve::OutCubic);
    //timeLine->setEasingCurve(QEasingCurve::OutQuart);
    readSettings();
    connect(timeLine, SIGNAL(frameChanged(int)),
            this, SLOT(doScroll(int)), Qt::UniqueConnection);
    // on scrolling animation finish
    connect(timeLine, SIGNAL(finished()), this, SIGNAL(scrolled()));
    // on manual scrollbar drag
    connect(scrollBar, SIGNAL(sliderMoved(int)), this, SIGNAL(scrolled()));
    connect(this, SIGNAL(scrolled()), frame, SIGNAL(scrolled()));
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    event->accept();
    viewportCenter = mapToScene(viewport()->rect().center());
    // on some systems pixelDelta() returns non-zero values with mouse wheel
    // that's why smoothScroll flag workaround
    if(!forceSmoothScroll && event->pixelDelta().y() != 0)  {
        // pixel scrolling (precise)
        doScroll(viewportCenter.x() - event->pixelDelta().y());
        emit scrolled();
    } else {
        // smooth scrolling by fixed intervals
        if(timeLine->state() == QTimeLine::Running) {
            timeLine->stop();
            timeLine->setFrameRange(viewportCenter.x(),
                                    timeLine->endFrame() -
                                    event->angleDelta().ry() *
                                    SCROLL_SPEED_MULTIPLIER);
        } else {
            timeLine->setFrameRange(viewportCenter.x(),
                                    viewportCenter.x() -
                                    event->angleDelta().ry() *
                                    SCROLL_SPEED_MULTIPLIER);
        }
        timeLine->setUpdateInterval(SCROLL_UPDATE_RATE);
        timeLine->start();
    }
}

void GraphicsView::doScroll(int dx) {
    centerOn(dx, viewportCenter.y());
}

void GraphicsView::readSettings() {
    forceSmoothScroll = settings->forceSmoothScroll();
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    ThumbnailLabel *item = qgraphicsitem_cast<ThumbnailLabel*>(itemAt(event->pos()));
    if(item) {
        frame->acceptThumbnailClick(item->labelNum());
    }
    QGraphicsView::mousePressEvent(event);
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

GraphicsView *ThumbnailFrame::view() const
{
    return graphicsView;
}

void ThumbnailFrame::addItem(QGraphicsItem *item)
{
    graphicsView->scene()->addItem(item);
}

void ThumbnailFrame::acceptThumbnailClick(int num) {
    emit thumbnailClicked(num);
}
