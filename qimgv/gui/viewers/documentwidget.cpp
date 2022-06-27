#include "documentwidget.h"

DocumentWidget::DocumentWidget(std::shared_ptr<ViewerWidget> viewWidget, std::shared_ptr<InfoBarProxy> infoBar, QWidget *parent)
    : FloatingWidgetContainer(parent),
      mainPanel(nullptr),
      mPanelPinned(false),
      mPanelEnabled(false),
      mPanelFullscreenOnly(false),
      avoidPanelFlag(false),
      mIsFullscreen(false),
      mInteractionEnabled(false),
      mAllowPanelInit(false)
{
    layoutRoot = new QVBoxLayout();
    layoutRoot->setContentsMargins(0,0,0,0);
    layoutRoot->setSpacing(0);
    layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layoutRoot->addLayout(layout);
    setLayout(layoutRoot);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    mViewWidget = viewWidget;
    mViewWidget->setParent(this);
    layout->addWidget(mViewWidget.get());
    mViewWidget.get()->show();
    mInfoBar = infoBar;
    mInfoBar->setParent(this);
    layoutRoot->addWidget(mInfoBar.get());
    setFocusProxy(mViewWidget.get());

    setInteractionEnabled(true);

    mainPanel.reset(new MainPanel(this));
    connect(mainPanel.get(), &MainPanel::pinned, this, &DocumentWidget::setPanelPinned);

    connect(settings, &Settings::settingsChanged, this, &DocumentWidget::readSettings);
    readSettings();
}

std::shared_ptr<ViewerWidget> DocumentWidget::viewWidget() {
    return mViewWidget;
}

void DocumentWidget::readSettings() {
    setPanelEnabled(settings->panelEnabled());
    mPanelFullscreenOnly = settings->panelFullscreenOnly();
    setPanelPinned(settings->panelPinned());
    mainPanel->readSettings();
}

void DocumentWidget::onFullscreenModeChanged(bool mode) {
    if(settings->panelPosition() == PANEL_TOP || settings->panelPosition() == PANEL_RIGHT)
        mainPanel->setExitButtonEnabled(mode);
    else
        mainPanel->setExitButtonEnabled(false);
    mIsFullscreen = mode;
}

void DocumentWidget::setPanelPinned(bool mode) {
    if(!mPanelEnabled)
        return;
    if(!mode) { // unpin
        if(mPanelPinned)
            layout->removeWidget(mainPanel.get());
        mainPanel->setLayoutManaged(false);
    } else {    // pin
        layout->insertWidget(1, mainPanel.get());
        switch(settings->panelPosition()) {
            case PANEL_TOP:
                layout->setDirection(QBoxLayout::BottomToTop); break;
            case PANEL_BOTTOM:
                layout->setDirection(QBoxLayout::TopToBottom); break;
            case PANEL_LEFT:
                layout->setDirection(QBoxLayout::RightToLeft); break;
            case PANEL_RIGHT:
                layout->setDirection(QBoxLayout::LeftToRight); break;
        }
        mainPanel->setLayoutManaged(true);
        mainPanel->show();
    }
    mPanelPinned = mode;
}

bool DocumentWidget::panelPinned() {
    return mPanelPinned;
}

std::shared_ptr<ThumbnailStripProxy> DocumentWidget::thumbPanel() {
    return mainPanel->getThumbnailStrip();
}

void DocumentWidget::hideFloatingPanel() {
    hideFloatingPanel(false);
}

void DocumentWidget::hideFloatingPanel(bool animated) {
    if(!mPanelPinned)
        mainPanel->hideAnimated();
}

void DocumentWidget::setPanelEnabled(bool mode) {
    mPanelEnabled = mode;
    if(!mode)
        mainPanel->hide();
    else
        setupMainPanel();
}

bool DocumentWidget::panelEnabled() {
    return mPanelEnabled;
}

void DocumentWidget::allowPanelInit() {
    mAllowPanelInit = true;
}

void DocumentWidget::setupMainPanel() {
    if(mPanelEnabled && mAllowPanelInit)
        mainPanel->setupThumbnailStrip();
}

void DocumentWidget::setInteractionEnabled(bool mode) {
    mInteractionEnabled = mode;
    if(!mode && !mPanelPinned)
        mainPanel->hide();
}

void DocumentWidget::mouseMoveEvent(QMouseEvent *event) {
    event->ignore();
    if(mPanelPinned)
        return;
    // ignore if we are doing something with the mouse (zoom / drag)
    if(event->buttons() != Qt::NoButton) {
        if(mainPanel->triggerRect().contains(event->pos()))
            avoidPanelFlag = true;
        return;
    }
    // show on hover event
    if(mPanelEnabled && (mIsFullscreen|| !mPanelFullscreenOnly)) {
        if(mainPanel->triggerRect().contains(event->pos()) && !avoidPanelFlag) {
            mainPanel->show();
        }
    }
    // fade out on leave event
    if(!mainPanel->isHidden()) {
        // leaveEvent which misfires on HiDPI (rounding error somewhere?)
        // add a few px of buffer area to avoid bugs
        // it still fcks up Fitts law as the buttons are not receiving hover on screen border

        // alright this also only works when in root window. sad.
        if(!mainPanel->triggerRect().adjusted(-8,-8,8,8).contains(event->pos())) {
            mainPanel->hideAnimated();
        }
    }
    if(!mainPanel->triggerRect().contains(event->pos()))
        avoidPanelFlag = false;
}

void DocumentWidget::enterEvent(QEnterEvent *event) {
    QWidget::enterEvent(event);
    if(!mInteractionEnabled)
        return;
    // we can't track move events outside the window (without additional timer),
    // so just hook the panel event here
    if(!mPanelPinned && mPanelEnabled && (mIsFullscreen || !mPanelFullscreenOnly)) {
        if(mainPanel->triggerRect().contains(mapFromGlobal(cursor().pos())) && !avoidPanelFlag)
            mainPanel->show();
    }
}

void DocumentWidget::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    //qDebug() << cursor().pos() << this->rect();
    // this misfires on hidpi.
    //instead do the panel hiding in MW::leaveEvent  (it works properly in root window)
    //mainPanel->hide();
    avoidPanelFlag = false;
}
