#include "mainpanel.h"

MainPanel::MainPanel(std::shared_ptr<QWidget> widget, OverlayContainerWidget *parent) : SlideHPanel(parent) {
    // buttons stuff
    buttonsWidget.setAccessibleName("panelButtonsWidget");

    openButton       = new IconButton("open", ":/res/icons/buttons/open20.png", 30);
    settingsButton   = new IconButton("openSettings", ":/res/icons/buttons/settings20.png", 30);
    exitButton       = new IconButton("exit", ":/res/icons/buttons/close16.png", 30);
    folderViewButton = new IconButton("folderView", ":/res/icons/buttons/folderview20.png", 30);
    //exitFullscreenButton = new IconButton("ToggleFullscreen", ":/res/icons/buttons/exit-fullscreen.png", QSize(30, 30));

    buttonsLayout.setDirection(QBoxLayout::BottomToTop);
    buttonsLayout.setSpacing(0);
    buttonsLayout.setContentsMargins(4,0,0,0);
    buttonsLayout.addWidget(settingsButton);
    buttonsLayout.addWidget(openButton);
    buttonsLayout.addStretch(0);
    //buttonsLayout.addWidget(&exitFullscreenButton);
    buttonsLayout.addWidget(folderViewButton);
    buttonsLayout.addWidget(exitButton);

    buttonsWidget.setLayout(&buttonsLayout);
    mLayout.addWidget(&buttonsWidget, 0, 1);

    setWidget(widget);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
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

void MainPanel::setWindowButtonsEnabled(bool mode) {
    exitButton->setHidden(!mode);
    //exitFullscreenButton.setHidden(!mode);
}

void MainPanel::readSettings() {
    setHeight(static_cast<int>(settings->mainPanelSize()));
    setPosition(settings->panelPosition());
}

// draw separator line at bottom or top
void MainPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter p(this);
    if(position == PanelHPosition::PANEL_TOP) {
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
