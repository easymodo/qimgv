#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip(QWidget *parent)
    : QWidget(parent),
      panelSize(122),
      current(-1),
      margin(2),
      thumbView(NULL),
      parentFullscreen(false)
{
    parentSz = parent->size();
    thumbnailLabels = new QList<ThumbnailLabel*>();

    thumbView = new ThumbnailView();
    widget = new ClickableWidget();

    thumbView->setWidget(widget);
    thumbView->setFrameShape(QFrame::NoFrame);

    viewLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    viewLayout->setSpacing(0);
    widget->setLayout(viewLayout);
    widget->setStyleSheet("background-color: #202020;"); // doesnt work from qss for some reason

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

    exitButton = new ClickableLabel();
    exitButton->setAccessibleName("panelButton");
    exitButton->setPixmap(QPixmap(":/images/res/icons/window-close.png"));

    buttonsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(0,0,0,0);
    buttonsLayout->addWidget(openButton);
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(settingsButton);
    buttonsLayout->addStretch(0);
    buttonsLayout->addWidget(exitButton);

    buttonsWidget->setLayout(buttonsLayout);

    // main layout
    layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(buttonsWidget);
    layout->addWidget(thumbView);

    this->setLayout(layout);

    // other extremely important things

    thumbView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    widget->setAccessibleName("thumbnailWidget");
    thumbView->setAccessibleName("thumbnailView");
    loadTimer.setSingleShot(true);
    timeLine = new QTimeLine(SCROLL_ANIMATION_SPEED, thumbView);
    timeLine->setCurveShape(QTimeLine::EaseInCurve);

    thumbView->horizontalScrollBar()->setAttribute(Qt::WA_NoMousePropagation, true);
    thumbView->verticalScrollBar()->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);
    thumbView->setFocusPolicy(Qt::NoFocus);

    // actions
    connect(openButton, SIGNAL(clicked()), this, SIGNAL(openClicked()));
    connect(saveButton, SIGNAL(clicked()), this, SIGNAL(saveClicked()));
    connect(settingsButton, SIGNAL(clicked()), this, SIGNAL(settingsClicked()));
    connect(exitButton, SIGNAL(clicked()), this, SIGNAL(exitClicked()));

    connect(widget, SIGNAL(pressedLeft(QPoint)), this, SLOT(viewPressed(QPoint)));
    connect(&loadTimer, SIGNAL(timeout()), this, SLOT(loadVisibleThumbnails()));
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));

    readSettings();
    this->hide();
}

void ThumbnailStrip::readSettings() {
    position = settings->panelPosition();
    panelSize = settings->thumbnailSize() + 21;
    scrollBar->setValue(0);
    updatePanelPosition();

    for(int i = 0; i < thumbnailLabels->count(); i++) {
        thumbnailLabels->at(i)->applySettings();
    }

    disconnect(timeLine, SIGNAL(frameChanged(int)), scrollBar, SLOT(setValue(int)));
    disconnect(scrollBar, SIGNAL(valueChanged(int)),
               this, SLOT(loadVisibleThumbnailsDelayed()));
    disconnect(scrollBar, SIGNAL(sliderMoved(int)),
               this, SLOT(loadVisibleThumbnailsDelayed()));
    exitButton->hide();
    if(position == LEFT || position == RIGHT) {
        thumbView->horizontalScrollBar()->setDisabled(true);
        thumbView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        thumbView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollBar = thumbView->verticalScrollBar();
        viewLayout->setDirection(QBoxLayout::TopToBottom);
        viewLayout->setContentsMargins(0, margin, 0, margin);
        buttonsLayout->setDirection(QBoxLayout::LeftToRight);
        if(position == LEFT)
            layout->setDirection(QBoxLayout::BottomToTop);
        else
            layout->setDirection(QBoxLayout::TopToBottom);
    } else {
        thumbView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        thumbView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollBar = thumbView->horizontalScrollBar();
        viewLayout->setDirection(QBoxLayout::LeftToRight);
        viewLayout->setContentsMargins(margin, 0, margin, 0);
        buttonsLayout->setDirection(QBoxLayout::BottomToTop);
        layout->setDirection(QBoxLayout::RightToLeft);
    }

    enableWindowControls(parentFullscreen);

    connect(scrollBar, SIGNAL(valueChanged(int)),
            this, SLOT(loadVisibleThumbnailsDelayed()), Qt::UniqueConnection);
    connect(scrollBar, SIGNAL(sliderMoved(int)),
            this, SLOT(loadVisibleThumbnailsDelayed()), Qt::UniqueConnection);
    connect(timeLine, SIGNAL(frameChanged(int)),
            scrollBar, SLOT(setValue(int)), Qt::UniqueConnection);
    widget->setFixedSize(viewLayout->sizeHint());
}

void ThumbnailStrip::populate(int count) {
    if(count >= 0 ) {
        widget->hide();
        for(int i = thumbnailLabels->count() - 1; i >= 0; --i) {
            delete viewLayout->takeAt(0);
            delete thumbnailLabels->takeAt(0);
        }
        // remove stretch
        viewLayout->takeAt(0);
        //recreate list
        delete thumbnailLabels;
        thumbnailLabels = new QList<ThumbnailLabel*>();

        for(int i = 0; i < count; i++) {
            addItem();
        }
        viewLayout->addStretch(1);
        widget->setFixedSize(viewLayout->sizeHint());
        widget->show();
    }
}

void ThumbnailStrip::fillPanel(int count) {
    if(count >= 0 ) {
        current = -1;
        loadTimer.stop();
        populate(count);
        loadVisibleThumbnailsDelayed();
    }
}

void ThumbnailStrip::addItem() {
    ThumbnailLabel *thumbLabel = new ThumbnailLabel();
    thumbLabel->setOpacity(0.0f);
    thumbnailLabels->append(thumbLabel);
    viewLayout->addWidget(thumbLabel);
}

void ThumbnailStrip::selectThumbnail(int pos) {
    if(current >= 0 && current < thumbnailLabels->count()) {
        thumbnailLabels->at(current)->setHighlighted(false);
        thumbnailLabels->at(current)->setOpacityAnimated(OPACITY_INACTIVE, ANIMATION_SPEED_INSTANT);
    }
    if(pos >= 0 && pos < thumbnailLabels->count()) {
        thumbnailLabels->at(pos)->setHighlighted(true);
        thumbnailLabels->at(pos)->setOpacityAnimated(OPACITY_SELECTED, ANIMATION_SPEED_INSTANT);
    }
    current = pos;
    loadVisibleThumbnails();
    focusOn(pos);
}

void ThumbnailStrip::focusOn(int pos) {
    if(pos >= 0 && pos < thumbnailLabels->count() && !childVisibleEntirely(pos)) {
        thumbView->ensureWidgetVisible(thumbnailLabels->at(pos), 350, 350);
    }
}

void ThumbnailStrip::loadVisibleThumbnailsDelayed() {
    loadTimer.stop();
    loadTimer.start(LOAD_DELAY);
}

void ThumbnailStrip::loadVisibleThumbnails() {
    loadTimer.stop();
    updateVisibleRegion();
    for(int i = 0; i < thumbnailLabels->count(); i++) {
        requestThumbnail(i);
    }
}

void ThumbnailStrip::requestThumbnail(int pos) {
    if(pos >= 0 && pos < thumbnailLabels->count()) {
        if(thumbnailLabels->at(pos)->state == EMPTY  && childVisible(pos)) {
            thumbnailLabels->at(pos)->state = LOADING;
            emit thumbnailRequested(pos);
        }
    }
}

void ThumbnailStrip::setThumbnail(int pos, Thumbnail *thumb) {
    thumbnailLabels->at(pos)->setThumbnail(thumb);
    thumbnailLabels->at(pos)->state = LOADED;
    if(pos != current) {
        thumbnailLabels->at(pos)->setOpacityAnimated(OPACITY_INACTIVE, ANIMATION_SPEED_NORMAL);
    }
}

void ThumbnailStrip::updateVisibleRegion() {
    // make scrollbar update while hidden
    thumbView->setWidgetResizable(true);
    layout->invalidate();
    layout->activate();

    if(viewLayout->direction() == QBoxLayout::LeftToRight) {
        visibleRegion = thumbView->rect().translated(scrollBar->value(), 0);
        preloadArea = visibleRegion.adjusted(-OFFSCREEN_PRELOAD_AREA, 0, OFFSCREEN_PRELOAD_AREA, 0);
    } else {
        visibleRegion = thumbView->rect().translated(0, scrollBar->value());
        preloadArea = visibleRegion.adjusted(0, -OFFSCREEN_PRELOAD_AREA, 0, OFFSCREEN_PRELOAD_AREA);
    }
}

bool ThumbnailStrip::childVisible(int pos) {
    if(pos >= 0 && pos < thumbnailLabels->count() &&
       preloadArea.intersects(viewLayout->itemAt(pos)->geometry()))
    {
        return true;
    }
    return false;
}

bool ThumbnailStrip::childVisibleEntirely(int pos) {
    if(pos >= 0 && pos < thumbnailLabels->count() &&
       visibleRegion.contains(viewLayout->itemAt(pos)->geometry().topLeft()) &&
       visibleRegion.contains(viewLayout->itemAt(pos)->geometry().bottomRight()))
    {
        return true;
    }
    return false;
}

// shows/hides exit button on the panel
void ThumbnailStrip::enableWindowControls(bool enabled) {
    if(enabled && (position == RIGHT || position == TOP))
        exitButton->show();
    else
        exitButton->hide();
    parentFullscreen = enabled;
}

void ThumbnailStrip::viewPressed(QPoint pos) {
    int itemPos = -1;
    // todo: quick search
    for (int i=0; i<viewLayout->count(); i++) {
        if(viewLayout->itemAt(i)->geometry().contains(pos)) {
            itemPos = i;
        }
    }
    if(itemPos != -1) {
        selectThumbnail(itemPos);
        emit thumbnailClicked(itemPos);
    }
}

void ThumbnailStrip::wheelEvent(QWheelEvent *event) {
    event->accept();
    if(event->pixelDelta().y() != 0)  { // pixel scrolling
        scrollBar->setValue(scrollBar->value() - event->pixelDelta().y());
    } else { // scrolling by fixed intervals
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
}

void ThumbnailStrip::parentResized(QSize parentSz) {
    this->parentSz = parentSz;
    updatePanelPosition();
    loadVisibleThumbnailsDelayed();
}

void ThumbnailStrip::updatePanelPosition() {
    QRect oldRect = this->rect();
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
    if(oldRect != this->rect())
        emit panelSizeChanged();
}

void ThumbnailStrip::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    hide();
}

ThumbnailStrip::~ThumbnailStrip() {
}
