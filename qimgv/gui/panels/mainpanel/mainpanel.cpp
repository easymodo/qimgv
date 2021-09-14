#include "mainpanel.h"

MainPanel::MainPanel(FloatingWidgetContainer *parent) : SlideHPanel(parent) {
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

    buttonsLayout.setDirection(QBoxLayout::BottomToTop);
    buttonsLayout.setSpacing(0);
    buttonsLayout.setContentsMargins(4,0,0,0);
    buttonsLayout.addWidget(settingsButton);
    buttonsLayout.addWidget(openButton);
    buttonsLayout.addStretch(0);
    buttonsLayout.addWidget(folderViewButton);
    buttonsLayout.addWidget(exitButton);

    buttonsWidget.setLayout(&buttonsLayout);
    mLayout.addWidget(&buttonsWidget, 0, 1);

    thumbnailStrip.reset(new ThumbnailStripProxy(this));
    setWidget(thumbnailStrip);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

MainPanel::~MainPanel() {
    delete openButton;
    delete settingsButton;
    delete exitButton;
    delete folderViewButton;
}

void MainPanel::setHeight(int newHeight) {
    if(panelHeight != newHeight) {
        panelHeight = newHeight;
        recalculateGeometry();
    }
}

void MainPanel::setPosition(PanelHPosition newPosition) {
    SlideHPanel::setPosition(newPosition);
    if(newPosition == PANEL_TOP)
        mLayout.setContentsMargins(0,0,0,bottomMargin);
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
}

void MainPanel::readSettings() {
    auto newPos = settings->panelPosition();
    int addedHeight = 19; // scrollbar & spacing
    if(newPos == PANEL_TOP)
        addedHeight = 16;
    setHeight(static_cast<int>(thumbnailStrip->itemSize().height() + addedHeight));
    setPosition(newPos);
}

// draw separator line at bottom or top
void MainPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter p(this);
    p.setPen(settings->colorScheme().folderview_hc);
    if(mPosition == PanelHPosition::PANEL_TOP) {
        p.drawLine(rect().bottomLeft() - QPoint(0, bottomMargin - 1), rect().bottomRight() - QPoint(0, bottomMargin - 1));
    } else {
        p.fillRect(rect().left(), rect().top(), width(), 3, settings->colorScheme().folderview);
        p.drawLine(rect().topLeft(), rect().topRight());
    }
}
