#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip(QWidget *parent)
    : QWidget(parent),
      panelSize(122),
      current(-1),
      thumbView(NULL)
{
    // setup scene & view
    thumbView = new ThumbnailView();
    widget = new QGraphicsWidget();
    scene = new CustomScene();

    scene->addItem(widget);
    widget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    widget->setMinimumHeight(300);

    thumbView->setScene(scene);
    thumbView->setFrameShape(QFrame::NoFrame);

    viewLayout = new QGraphicsLinearLayout();
    viewLayout->addStretch(1);
    viewLayout->setSpacing(0);
    widget->setLayout(viewLayout);

    scrollBar = thumbView->horizontalScrollBar();

    // BUTTONS
    buttonsWidget = new QWidget();
    buttonsWidget->setAccessibleName("panelButtonsWidget");

    openButton = new ClickableLabel();
    openButton->setAccessibleName("panelButton");
    openButton->setPixmap(QPixmap(":/images/res/icons/open.png"));

    saveButton = new ClickableLabel();
    saveButton->setAccessibleName("panelButton");
    saveButton->setPixmap(QPixmap(":/images/res/icons/save.png"));

    settingsButton = new ClickableLabel();
    settingsButton->setAccessibleName("panelButton");
    settingsButton->setPixmap(QPixmap(":/images/res/icons/settings.png"));

    buttonsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(0,0,0,0);
    buttonsLayout->addWidget(openButton);
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(settingsButton);
    buttonsLayout->addStretch(0);

    buttonsWidget->setLayout(buttonsLayout);

    // main layout
    layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(buttonsWidget);
    layout->addWidget(thumbView);

    this->setLayout(layout);

    // other extremely important things
    thumbView->setAccessibleName("thumbnailGraphicsView");
    loadTimer.setSingleShot(true);
    timeLine = new QTimeLine(SCROLL_ANIMATION_SPEED, thumbView);
    timeLine->setCurveShape(QTimeLine::EaseInCurve);

    thumbView->horizontalScrollBar()->setAttribute(Qt::WA_NoMousePropagation, true);
    thumbView->verticalScrollBar()->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);
    thumbView->setFocusPolicy(Qt::NoFocus);
    //this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    //thumbView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum));

    // actions
    connect(openButton, SIGNAL(clicked()), this, SIGNAL(openClicked()));
    connect(saveButton, SIGNAL(clicked()), this, SIGNAL(saveClicked()));
    connect(settingsButton, SIGNAL(clicked()), this, SIGNAL(settingsClicked()));

    connect(scene, SIGNAL(sceneClick(QPointF)), this, SLOT(sceneClicked(QPointF)));
    connect(&loadTimer, SIGNAL(timeout()), this, SLOT(loadVisibleThumbnails()));
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
    this->show();
    readSettings();
   // this->hide();
}

void ThumbnailStrip::readSettings() {
    position = settings->panelPosition();
    panelSize = settings->thumbnailSize() + 22;
    scrollBar->setValue(0);

    for(int i = 0; i < thumbnailLabels.count(); i++) {
        thumbnailLabels.at(i)->applySettings();
    }

    disconnect(timeLine, SIGNAL(frameChanged(int)), scrollBar, SLOT(setValue(int)));
    disconnect(scrollBar, SIGNAL(valueChanged(int)),
               this, SLOT(loadVisibleThumbnailsDelayed()));
    disconnect(scrollBar, SIGNAL(sliderMoved(int)),
               this, SLOT(loadVisibleThumbnailsDelayed()));
    if(position == LEFT || position == RIGHT) {
        thumbView->horizontalScrollBar()->setDisabled(true);
        thumbView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        thumbView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollBar = thumbView->verticalScrollBar();
        viewLayout->setOrientation(Qt::Vertical);
        viewLayout->setContentsMargins(0, 2, 0, 2);
        buttonsLayout->setDirection(QBoxLayout::LeftToRight);
        layout->setDirection(QBoxLayout::BottomToTop);
    } else {
        thumbView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        thumbView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollBar = thumbView->horizontalScrollBar();
        viewLayout->setOrientation(Qt::Horizontal);
        viewLayout->setContentsMargins(2, 0, 2, 0);
        buttonsLayout->setDirection(QBoxLayout::TopToBottom);
        layout->setDirection(QBoxLayout::RightToLeft);
    }

    connect(scrollBar, SIGNAL(valueChanged(int)),
            this, SLOT(loadVisibleThumbnailsDelayed()), Qt::UniqueConnection);
    connect(scrollBar, SIGNAL(sliderMoved(int)),
            this, SLOT(loadVisibleThumbnailsDelayed()), Qt::UniqueConnection);
    connect(timeLine, SIGNAL(frameChanged(int)),
            scrollBar, SLOT(setValue(int)), Qt::UniqueConnection);
    viewLayout->invalidate();
    viewLayout->activate();
    //scene->setSceneRect(scene->itemsBoundingRect());
    //scene->setSceneRect(viewLayout->geometry());
    qDebug() << "readSettings: scenerect = " << scene->sceneRect();
}

void ThumbnailStrip::populate(int count) {
    // this will fail if list items != layout items
    // shouldnt happen though
    for(int i = viewLayout->count() - 1; i >= 0; --i) {
        viewLayout->removeAt(0);
        delete thumbnailLabels.takeAt(0);
    }
    qDebug() << viewLayout->geometry();
    thumbnailLabels.clear();

    for(int i = 0; i < count; i++) {
        addItem();
    }

    viewLayout->invalidate();
    viewLayout->activate();
    viewLayout->setGeometry(viewLayout->geometry().translated(0,-5));
    viewLayout->setContentsMargins(0,0,0,0);
    qDebug() << "layout geom." << viewLayout->geometry();
    scene->setSceneRect(viewLayout->geometry());
    //scene->addRect(0,0, scene->width(), scene->height(),QPen(QColor(Qt::green)),QBrush(Qt::SolidPattern));
    qDebug() << "VIEW:::" << thumbView->rect();
    qDebug() << "SCENE:::" << scene->sceneRect();
}

void ThumbnailStrip::fillPanel(int count) {
    current = -1;
    loadTimer.stop();
    populate(count);
    loadVisibleThumbnailsDelayed();
}

void ThumbnailStrip::addItem() {
    ThumbnailLabel *thumbLabel = new ThumbnailLabel();
    thumbLabel->setOpacityAnimated(0.0, ANIMATION_SPEED_INSTANT);
    thumbnailLabels.append(thumbLabel);
    viewLayout->addItem(thumbLabel);
    requestThumbnail(thumbnailLabels.length() - 1);
}

void ThumbnailStrip::selectThumbnail(int pos) {
    if(current != -1) {
        thumbnailLabels.at(current)->setHighlighted(false);
        thumbnailLabels.at(current)->setOpacityAnimated(OPACITY_INACTIVE, ANIMATION_SPEED_INSTANT);
    }
    thumbnailLabels.at(pos)->setHighlighted(true);
    thumbnailLabels.at(pos)->setOpacityAnimated(OPACITY_SELECTED, ANIMATION_SPEED_FAST);
    current = pos;
    if(!childVisibleEntirely(pos)) {
        thumbView->centerOn(thumbnailLabels.at(pos)->scenePos());
    }
    loadVisibleThumbnails();
}

void ThumbnailStrip::translateX(int dx) {
    thumbView->translate((qreal) dx / 10, 0);
}

// in theory faster than scene's version
QRectF ThumbnailStrip::itemsBoundingRect() {
    QRectF boundingRect(0, 0, 0, 0);
    if(!thumbnailLabels.isEmpty()) {
        boundingRect.setTopLeft(
            thumbnailLabels.at(0)->sceneBoundingRect().topLeft());
        boundingRect.setBottomRight(
            thumbnailLabels.at(thumbnailLabels.count() - 1)->sceneBoundingRect().bottomRight());
    }
    return boundingRect;
}

void ThumbnailStrip::loadVisibleThumbnailsDelayed() {
    loadTimer.stop();
    loadTimer.start(LOAD_DELAY);
}

void ThumbnailStrip::loadVisibleThumbnails() {
    loadTimer.stop();
    updateVisibleRegion();

    for(int i = 0; i < thumbnailLabels.count(); i++) {
        requestThumbnail(i);
    }
}

void ThumbnailStrip::requestThumbnail(int pos) {
    if(thumbnailLabels.at(pos)->state == EMPTY  && childVisible(pos)) {
        thumbnailLabels.at(pos)->state = LOADING;
        emit thumbnailRequested(pos);
    }
}

void ThumbnailStrip::setThumbnail(int pos, Thumbnail *thumb) {
    thumbnailLabels.at(pos)->setThumbnail(thumb);
    thumbnailLabels.at(pos)->state = LOADED;
    if(pos != current) {
        thumbnailLabels.at(pos)->setOpacityAnimated(OPACITY_INACTIVE, ANIMATION_SPEED_NORMAL);
    }
}

void ThumbnailStrip::updateVisibleRegion() {
    QRect viewport_rect(0, 0, thumbView->width(), thumbView->height());
    preloadArea = visibleRegion = thumbView->mapToScene(viewport_rect).boundingRect();
    if(viewLayout->orientation() == Qt::Vertical) {
        preloadArea.adjust(0, -OFFSCREEN_PRELOAD_AREA, 0, OFFSCREEN_PRELOAD_AREA);
    } else {
        preloadArea.adjust(-OFFSCREEN_PRELOAD_AREA, 0, OFFSCREEN_PRELOAD_AREA, 0);
    }
}

bool ThumbnailStrip::childVisible(int pos) {
    if(thumbnailLabels.count() > pos) {
        return thumbnailLabels.at(pos)->
               sceneBoundingRect().intersects(preloadArea);
    } else {
        return false;
    }
}

bool ThumbnailStrip::childVisibleEntirely(int pos) {
    if(thumbnailLabels.count() > pos) {
        return thumbnailLabels.at(pos)->
               sceneBoundingRect().intersects(visibleRegion);
    } else {
        return false;
    }
}

void ThumbnailStrip::sceneClicked(QPointF pos) {
    ThumbnailLabel *item = (ThumbnailLabel *) scene->itemAt(pos, thumbView->transform());
    int itemPos = thumbnailLabels.indexOf(item);
    if(itemPos != -1) {
        emit thumbnailClicked(itemPos);
    }
}

void ThumbnailStrip::wheelEvent(QWheelEvent *event) {
    qDebug() << thumbView->geometry();
    event->accept();
    if(timeLine->state() == QTimeLine::Running) {
        timeLine->stop();
        timeLine->setFrameRange(scrollBar->value(),
                                timeLine->endFrame() -
                                event->angleDelta().ry() * SCROLL_SPEED_MULTIPLIER);
    } else {
        timeLine->setFrameRange(scrollBar->value(),
                                scrollBar->value() -
                                event->angleDelta().ry() * SCROLL_SPEED_MULTIPLIER);
    }
    timeLine->setUpdateInterval(SCROLL_UPDATE_RATE);
    timeLine->start();
}

void ThumbnailStrip::parentResized(QSize parentSz) {
    if(position == BOTTOM) {
        this->setGeometry(QRect(QPoint(0, parentSz.height() - panelSize + 1),
                                QPoint(parentSz.width(), parentSz.height())));
    } else if(position == LEFT) {
        this->setGeometry(QRect(QPoint(0, 0),
                                QPoint(panelSize, parentSz.height())));
    } else if(position == RIGHT) {
        this->setGeometry(QRect(QPoint(parentSz.width() - panelSize, 0),
                                QPoint(parentSz.width(), parentSz.height())));
    } else if(position == TOP) {
        this->setGeometry(QRect(QPoint(0, 0),
                                QPoint(parentSz.width(), panelSize)));
    }
    loadVisibleThumbnailsDelayed();
}

void ThumbnailStrip::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    hide();
}

ThumbnailStrip::~ThumbnailStrip() {
}
