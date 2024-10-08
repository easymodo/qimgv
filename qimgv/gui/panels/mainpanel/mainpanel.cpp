#include "mainpanel.h"

MainPanel::MainPanel(FloatingWidgetContainer *parent) : SlidePanel(parent) {
    // buttons stuff
    buttonsWidget.setAccessibleName("panelButtonsWidget");
    openButton       = new ActionButton("open", ":res/icons/common/buttons/panel/open20.png", 30, this);
    openButton->setAccessibleName("ButtonSmall");
    openButton->setTriggerMode(TriggerMode::PressTrigger);
    settingsButton   = new ActionButton("openSettings", ":res/icons/common/buttons/panel/settings20.png", 30, this);
    settingsButton->setAccessibleName("ButtonSmall");
    settingsButton->setTriggerMode(TriggerMode::PressTrigger);
    exitButton       = new ActionButton("exit", ":res/icons/common/buttons/panel/close16.png", 30, this);
    exitButton->setAccessibleName("ButtonSmall");
    exitButton->setTriggerMode(TriggerMode::PressTrigger);
    folderViewButton = new ActionButton("folderView", ":res/icons/common/buttons/panel/folderview20.png", 30, this);
    folderViewButton->setAccessibleName("ButtonSmall");
    folderViewButton->setTriggerMode(TriggerMode::PressTrigger);
    pinButton = new ActionButton("", ":res/icons/common/buttons/panel/pin-panel20.png", 30, this);
    pinButton->setAccessibleName("ButtonSmall");
    pinButton->setTriggerMode(TriggerMode::PressTrigger);
    pinButton->setCheckable(true);
    connect(pinButton, &ActionButton::toggled, this, &MainPanel::onPinClicked);

    buttonsLayout.setDirection(QBoxLayout::BottomToTop);
    buttonsLayout.setSpacing(0);
    buttonsLayout.addWidget(settingsButton);
    buttonsLayout.addWidget(openButton);
    buttonsLayout.addStretch(0);
    buttonsLayout.addWidget(pinButton);
    buttonsLayout.addWidget(folderViewButton);
    buttonsLayout.addWidget(exitButton);

    buttonsWidget.setLayout(&buttonsLayout);

    layout()->addWidget(&buttonsWidget);

    thumbnailStrip.reset(new ThumbnailStripProxy(this));
    setWidget(thumbnailStrip);

    readSettings();
    //connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

MainPanel::~MainPanel() {
    delete openButton;
    delete settingsButton;
    delete exitButton;
    delete folderViewButton;
}

void MainPanel::onPinClicked() {
    bool mode = !settings->panelPinned();
    pinButton->setChecked(mode);
    settings->setPanelPinned(mode);
    emit pinned(mode);
}

void MainPanel::setPosition(PanelPosition p) {
    SlidePanel::setPosition(p);
    switch(p) {
        case PANEL_TOP:
            buttonsLayout.setDirection(QBoxLayout::BottomToTop);
            layout()->setContentsMargins(0,0,0,1);
            buttonsLayout.setContentsMargins(4,0,0,0);
        break;
        case PANEL_BOTTOM:
            buttonsLayout.setDirection(QBoxLayout::BottomToTop);
            layout()->setContentsMargins(0,3,0,0);
            buttonsLayout.setContentsMargins(4,0,0,0);
        break;
        case PANEL_LEFT:
            buttonsLayout.setDirection(QBoxLayout::LeftToRight);
            layout()->setContentsMargins(0,0,1,0);
            buttonsLayout.setContentsMargins(0,0,0,4);
        break;
        case PANEL_RIGHT:
            buttonsLayout.setDirection(QBoxLayout::LeftToRight);
            layout()->setContentsMargins(1,0,0,0);
            buttonsLayout.setContentsMargins(0,0,0,4);
        break;
    }
    recalculateGeometry();
}

void MainPanel::setExitButtonEnabled(bool mode) {
    exitButton->setHidden(!mode);
}

std::shared_ptr<ThumbnailStripProxy> MainPanel::getThumbnailStrip() {
    return thumbnailStrip;
}

void MainPanel::setupThumbnailStrip() {
    thumbnailStrip->init();
    // adjust size & position
    readSettings();
}

QSize MainPanel::sizeHint() const {
    if(!thumbnailStrip->isInitialized())
        return QSize(0, 0);
    // item size + spacing + scrollbar + border
    switch(settings->panelPosition()) {
        case PANEL_TOP:
            return QSize(width(), thumbnailStrip->itemSize().height() + 16);
        case PANEL_BOTTOM:
            return QSize(width(), thumbnailStrip->itemSize().height() + 16 + 3);
        case PANEL_LEFT:
        case PANEL_RIGHT:
            return QSize(thumbnailStrip->itemSize().width() + 16, height());
        default:
            return QSize(0, 0);
    }
}

void MainPanel::readSettings() {
    auto newPos = settings->panelPosition();
    if(newPos == PANEL_TOP || newPos == PANEL_BOTTOM) {
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        int h = sizeHint().height();
        if(h)
            setFixedHeight(h);
        setFixedWidth(QWIDGETSIZE_MAX);
    } else {
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        int w = sizeHint().width();
        if(w)
            setFixedWidth(w);
        setFixedHeight(QWIDGETSIZE_MAX);
    }
    thumbnailStrip->readSettings();
    setPosition(newPos);
    pinButton->setChecked(settings->panelPinned());
}

// draw separator line at bottom or top
void MainPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    // borders
    QPainter p(this);
    p.setPen(settings->colorScheme().folderview_hc);
    switch(mPosition) {
        case PANEL_TOP:
            p.drawLine(rect().bottomLeft(), rect().bottomRight());
        break;
        case PANEL_BOTTOM:
            p.fillRect(rect().left(), rect().top(), width(), 3, settings->colorScheme().folderview);
            p.drawLine(rect().topLeft(), rect().topRight());
        break;
        case PANEL_LEFT:
            p.drawLine(rect().topRight(), rect().bottomRight());
        break;
        case PANEL_RIGHT:
            p.drawLine(rect().topLeft(), rect().bottomLeft());
        break;
    }
}
