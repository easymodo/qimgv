#include "mainpanel.h"

MainPanel::MainPanel(FloatingWidgetContainer *parent) : SlideHPanel(parent) {
    // buttons stuff
    buttonsWidget.setAccessibleName("panelButtonsWidget");
    auto icontheme = settings->theme().iconTheme;
    openButton       = new ActionButton("open", ":res/icons/" + icontheme + "/buttons/panel/open20.png", 30, this);
    openButton->setAccessibleName("PanelButtonSmall");
    openButton->setTriggerMode(TriggerMode::PressTrigger);
    settingsButton   = new ActionButton("openSettings", ":res/icons/" + icontheme + "/buttons/panel/settings20.png", 30, this);
    settingsButton->setAccessibleName("PanelButtonSmall");
    settingsButton->setTriggerMode(TriggerMode::PressTrigger);
    exitButton       = new ActionButton("exit", ":res/icons/" + icontheme + "/buttons/panel/close16.png", 30, this);
    exitButton->setAccessibleName("PanelButtonSmall");
    exitButton->setTriggerMode(TriggerMode::PressTrigger);
    folderViewButton = new ActionButton("folderView", ":res/icons/" + icontheme + "/buttons/panel/folderview20.png", 30, this);
    folderViewButton->setAccessibleName("PanelButtonSmall");
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

    thumbnailStrip.reset(new ThumbnailStrip());
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
    if(newPosition == PANEL_TOP) {
        mLayout.setContentsMargins(0,0,0,bottomMargin);
    } else {
        mLayout.setContentsMargins(0,3,0,0);
    }
    recalculateGeometry();
}

void MainPanel::setExitButtonEnabled(bool mode) {
    exitButton->setHidden(!mode);
}

std::shared_ptr<ThumbnailStrip> MainPanel::getThumbnailStrip() {
    return thumbnailStrip;
}

void MainPanel::readSettings() {
    setHeight(static_cast<int>(settings->mainPanelSize()));
    setPosition(settings->panelPosition());
}

// draw separator line at bottom or top
void MainPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter p(this);
    if(mPosition == PanelHPosition::PANEL_TOP) {
        p.setPen(QColor(QColor(96, 96, 96)));
        p.drawLine(rect().bottomLeft() - QPoint(0, bottomMargin - 1), rect().bottomRight() - QPoint(0, bottomMargin - 1));
        p.setPen(QColor(QColor(40, 40, 40)));
        p.drawLine(rect().bottomLeft() - QPoint(0, bottomMargin - 2), rect().bottomRight() - QPoint(0, bottomMargin - 2));
    } else {
        p.setPen(QColor(QColor(40, 40, 40)));
        p.drawLine(rect().topLeft(), rect().topRight());
        p.drawLine(rect().topLeft() + QPoint(0,1), rect().topRight() + QPoint(0,1));
        p.drawLine(rect().topLeft() + QPoint(0,2), rect().topRight() + QPoint(0,2));
        p.drawLine(rect().topLeft() + QPoint(0,3), rect().topRight() + QPoint(0,3));
    }
}
