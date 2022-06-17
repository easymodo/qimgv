#include "mainpanel.h"

MainPanel::MainPanel(FloatingWidgetContainer *parent) : SlideHPanel(parent) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
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
    buttonsLayout.setContentsMargins(4,0,0,0);
    buttonsLayout.addWidget(settingsButton);
    buttonsLayout.addWidget(openButton);
    buttonsLayout.addStretch(0);
    buttonsLayout.addWidget(pinButton);
    buttonsLayout.addWidget(folderViewButton);
    buttonsLayout.addWidget(exitButton);

    buttonsWidget.setLayout(&buttonsLayout);
    mLayout.addWidget(&buttonsWidget, 0, 1);

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

void MainPanel::setPosition(PanelHPosition newPosition) {
    SlideHPanel::setPosition(newPosition);
    // add a few px for drawing border
    if(newPosition == PANEL_TOP)
        mLayout.setContentsMargins(0,0,0,1);
    else
        mLayout.setContentsMargins(0,3,0,0);
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
    auto newPos = settings->panelPosition();
    int addedHeight = 19; // (thumb <> scrollbar) spacing + scrollbar + border
    if(newPos == PANEL_TOP)
        addedHeight = 16;
    return QSize(width(), static_cast<int>(thumbnailStrip->itemSize().height() + addedHeight));
}

void MainPanel::readSettings() {
    pinButton->setChecked(settings->panelPinned());
    thumbnailStrip->readSettings();
    int h = sizeHint().height();
    if(h)
        setFixedHeight(h);
    setPosition(settings->panelPosition());
}

// draw separator line at bottom or top
void MainPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter p(this);
    p.setPen(settings->colorScheme().folderview_hc);
    if(mPosition == PanelHPosition::PANEL_TOP) {
        p.drawLine(rect().bottomLeft(), rect().bottomRight());
    } else {
        p.fillRect(rect().left(), rect().top(), width(), 3, settings->colorScheme().folderview);
        p.drawLine(rect().topLeft(), rect().topRight());
    }
}
