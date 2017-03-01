#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip()
    : panelSize(122),
      current(-1),
      margin(2),
      thumbnailFrame(NULL),
      parentFullscreen(false),
      idCounter(0)
{
    thumbnailLabels = new QList<ThumbnailLabel*>();

    thumbnailFrame = new ThumbnailFrame();
    scene = new QGraphicsScene(this); // move scene to view class?
    thumbnailFrame->view()->setScene(scene);
    thumbnailFrame->setFrameShape(QFrame::NoFrame);

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
    layout->addWidget(thumbnailFrame);

    this->setLayout(layout);

    // other extremely important things
    thumbnailFrame->setAccessibleName("thumbnailView");
    loadTimer.setSingleShot(true);

    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setFocusPolicy(Qt::NoFocus);

    connect(thumbnailFrame,SIGNAL(thumbnailClicked(int)),
            this, SLOT(thumbnailClicked(int)));

    // actions
    connect(openButton, SIGNAL(clicked()), this, SIGNAL(openClicked()));
    connect(saveButton, SIGNAL(clicked()), this, SIGNAL(saveClicked()));
    connect(settingsButton, SIGNAL(clicked()), this, SIGNAL(settingsClicked()));
    connect(exitButton, SIGNAL(clicked()), this, SIGNAL(exitClicked()));

    connect(&loadTimer, SIGNAL(timeout()), this, SLOT(loadVisibleThumbnails()));
    connect(thumbnailFrame, SIGNAL(scrolled()), this, SLOT(loadVisibleThumbnailsDelayed()));
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));

    readSettings();
}

void ThumbnailStrip::readSettings() {
    thumbnailSize = settings->thumbnailSize();
    position = settings->panelPosition();
    if(position == PanelPosition::TOP)
        layout->setContentsMargins(0,0,0,1);
    panelSize = settings->thumbnailSize() + 22;
    this->setGeometry(QRect(QPoint(0, 0),
                      QPoint(width(), panelSize)));

    for(int i = 0; i < thumbnailLabels->count(); i++) {
        if(thumbnailLabels->at(i)->getThumbnailSize() != thumbnailSize) {
            // force thumbnail recreate
            thumbnailLabels->at(i)->state = loadState::EMPTY;
        }
        thumbnailLabels->at(i)->readSettings();
    }

    updateThumbnailPositions(0, thumbnailLabels->count() - 1);

    buttonsLayout->setDirection(QBoxLayout::BottomToTop);
    layout->setDirection(QBoxLayout::RightToLeft);
    //loadVisibleThumbnails();
}

void ThumbnailStrip::populate(int count) {
    if(count >= 0 ) {
        for(int i = thumbnailLabels->count() - 1; i >= 0; --i) {
            delete thumbnailLabels->takeAt(0);
        }
        scene->clear();
        //recreate list
        delete thumbnailLabels;
        thumbnailLabels = new QList<ThumbnailLabel*>();

        for(int i = 0; i < count; i++) {
            addItem();
        }
        shrinkScene();
        thumbnailFrame->view()->resetViewport();
    }
}

void ThumbnailStrip::fillPanel(int count) {
    if(count >= 0 ) {
        current = -1;
        loadTimer.stop();
        populate(count);
        loadVisibleThumbnails();
    }
}

void ThumbnailStrip::addItem() {
    addItemAt(thumbnailLabels->count());
}

void ThumbnailStrip::addItemAt(int pos) {
    lock();
    ThumbnailLabel *thumbLabel = new ThumbnailLabel();
    thumbLabel->setOpacity(0);
    thumbnailLabels->insert(pos, thumbLabel);
    thumbLabel->setLabelNum(pos);
    scene->addItem(thumbLabel);

    // set the id
    posIdHash.insert(pos, idCounter);
    posIdHashReverse.insert(idCounter, pos);
    idCounter++;

    // set position for new & move existing items right
    updateThumbnailPositions(pos, thumbnailLabels->count() - 1);
    unlock();
}

void ThumbnailStrip::parentResized(QSize parentSz) {
    resize(parentSz.width(), height());
    loadVisibleThumbnailsDelayed();
}

void ThumbnailStrip::updateThumbnailPositions(int start, int end) {
    if(start > end || !checkRange(start) || !checkRange(end)) {
        qDebug() << "ThumbnailStrip::updateThumbnailPositions() - arguments out of range";
        return;
    }
    // assume all thumbnails are the same size
    int thumbWidth = thumbnailLabels->at(start)->boundingRect().width();
    ThumbnailLabel *tmp;
    for(int i = start; i <= end; i++) {
        tmp = thumbnailLabels->at(i);
        tmp->setPos(i * thumbWidth, 0);
    }
}

// shrink scene to contents. use after deleting items
void ThumbnailStrip::shrinkScene() {
    scene->setSceneRect(scene->itemsBoundingRect());
}

void ThumbnailStrip::selectThumbnail(int pos) {
    // this code fires twice on click. fix later
    // also wont highlight new label after removing file
    if(current != pos) {
        thumbnailFrame->view()->ensureVisible(thumbnailLabels->at(pos)->sceneBoundingRect(),
                                              thumbnailSize / 2,
                                              0);
        if(checkRange(current)) {
            thumbnailLabels->at(current)->setHighlighted(false);
            thumbnailLabels->at(current)->setOpacityAnimated(OPACITY_INACTIVE, ANIMATION_SPEED_FAST);
        }
        if(checkRange(pos)) {
            thumbnailLabels->at(pos)->setHighlighted(true);
            thumbnailLabels->at(pos)->setOpacity(OPACITY_SELECTED);
            current = pos;
        }
    }
    loadVisibleThumbnails();
}

void ThumbnailStrip::thumbnailClicked(int pos) {
    selectThumbnail(pos);
    emit openImage(pos);
}

void ThumbnailStrip::loadVisibleThumbnailsDelayed() {
    loadTimer.stop();
    loadTimer.start(LOAD_DELAY);
}

void ThumbnailStrip::loadVisibleThumbnails() {
    QRectF visibleRect = thumbnailFrame->view()->mapToScene(
                thumbnailFrame->view()->viewport()->geometry()).boundingRect();
    // grow rectangle to cover nearby offscreen items
    visibleRect.adjust(-OFFSCREEN_PRELOAD_AREA, 0, OFFSCREEN_PRELOAD_AREA, 0);

    QList<QGraphicsItem *>items = scene->items(visibleRect,
                                               Qt::IntersectsItemShape,
                                               Qt::DescendingOrder
                                               );
    ThumbnailLabel* labelCurrent;
    loadTimer.stop();
    for(int i = 0; i < items.count(); i++) {
        labelCurrent = qgraphicsitem_cast<ThumbnailLabel*>(items.at(i));
        requestThumbnail(labelCurrent->labelNum());
    }
}

void ThumbnailStrip::requestThumbnail(int pos) {
    if(checkRange(pos) && thumbnailLabels->at(pos)->state == EMPTY) {
            thumbnailLabels->at(pos)->state = LOADING;
            emit thumbnailRequested(pos, posIdHash.value(pos));
    }
}

bool ThumbnailStrip::checkRange(int pos) {
    if(pos >= 0 && pos < thumbnailLabels->count())
        return true;
    else
        return false;
}

void ThumbnailStrip::lock() {
    mutex.lock();
}

void ThumbnailStrip::unlock() {
    mutex.unlock();
}

void ThumbnailStrip::setThumbnail(long thumbnailId, Thumbnail *thumb) {
    lock();
    int pos = posIdHashReverse.value(thumbnailId);
    if(checkRange(pos)) {
        thumbnailLabels->at(pos)->setThumbnail(thumb);
        thumbnailLabels->at(pos)->state = LOADED;
        if(pos != current) {
            thumbnailLabels->at(pos)->setOpacityAnimated(OPACITY_INACTIVE, ANIMATION_SPEED_NORMAL);
        }
    }
    unlock();
}

// shows/hides exit button on the panel
void ThumbnailStrip::setWindowControlsEnabled(bool enabled) {
    if(enabled && position == TOP)
        exitButton->show();
    else
        exitButton->hide();
    parentFullscreen = enabled;
}

void ThumbnailStrip::removeItemAt(int pos) {
    lock();
    if(checkRange(pos)) {
        ThumbnailLabel *thumb = thumbnailLabels->takeAt(pos);
        scene->removeItem(thumb);
        long thumbnailId = posIdHash.value(pos);
        posIdHash.remove(pos);
        posIdHashReverse.remove(thumbnailId);
        // move items left
        ThumbnailLabel *tmp;
        for(int i = pos; i < thumbnailLabels->count(); i++) {
            tmp = thumbnailLabels->at(i);
            tmp->moveBy(-tmp->boundingRect().width(), 0);
            tmp->setLabelNum(i);
            // update hash (keep thumbnailId, set new position)
            thumbnailId = posIdHash.value(i+1);
            posIdHash.remove(i+1);
            posIdHash.insert(i, thumbnailId);
            posIdHashReverse.remove(thumbnailId);
            posIdHashReverse.insert(thumbnailId, tmp->labelNum());
        }
        shrinkScene();
    }
    unlock();
}

void ThumbnailStrip::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if(position == PanelPosition::TOP) {
        QPainter p(this);
        p.setPen(QColor(QColor(110, 110, 110)));
        p.drawLine(rect().bottomLeft(), rect().bottomRight());
    }
}

void ThumbnailStrip::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    loadVisibleThumbnailsDelayed();
}

ThumbnailStrip::~ThumbnailStrip() {
}
