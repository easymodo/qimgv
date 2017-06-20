#include "mainpanel.h"

MainPanel::MainPanel(QWidget *parent) : SlideHPanel(parent) {
    // buttons stuff
    buttonsWidget.setAccessibleName("panelButtonsWidget");

    openButton.setPixmap(QPixmap(":/res/icons/buttons/open20.png"));
    openButton.setAction("open");
    settingsButton.setPixmap(QPixmap(":/res/icons/buttons/settings20.png"));
    settingsButton.setAction("openSettings");
    //exitFullscreenButton.setPixmap(QPixmap(":/res/icons/exit-fullscreen.png"));
    //exitFullscreenButton.setAction("toggleFullscreen");
    exitButton.setPixmap(QPixmap(":/res/icons/buttons/close20.png"));
    exitButton.setAction("exit");

    buttonsLayout.setDirection(QBoxLayout::BottomToTop);
    buttonsLayout.setSpacing(0);
    buttonsLayout.setContentsMargins(3,0,0,0);
    buttonsLayout.addWidget(&settingsButton);
    buttonsLayout.addWidget(&openButton);
    buttonsLayout.addStretch(0);
    //buttonsLayout.addWidget(&exitFullscreenButton);
    buttonsLayout.addWidget(&exitButton);

    buttonsWidget.setLayout(&buttonsLayout);
    mLayout.addWidget(&buttonsWidget, 0, 1);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void MainPanel::setPosition(PanelHPosition newPosition) {
    position = newPosition;
    if(position == PANEL_TOP) {
        mLayout.setContentsMargins(0,0,0,invisibleMargin);
    }
    else {
        mLayout.setContentsMargins(0,3,0,0);
    }
    recalculateGeometry();
}

void MainPanel::setWidget(QWidget *w) {
    SlidePanel::setWidget(w);
}

void MainPanel::setWindowButtonsEnabled(bool mode) {
    exitButton.setHidden(!mode);
    //exitFullscreenButton.setHidden(!mode);
}

void MainPanel::readSettings() {
    panelHeight = settings->mainPanelSize();
    setPosition(settings->panelPosition());
}

// draw separator line at bottom or top
void MainPanel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter p(this);
    if(position == PanelHPosition::PANEL_TOP) {
        p.setPen(QColor(QColor(110, 110, 110)));
        p.drawLine(rect().bottomLeft() - QPoint(0, invisibleMargin - 1), rect().bottomRight() - QPoint(0, invisibleMargin - 1));
        p.setPen(QColor(QColor(40, 40, 40)));
        p.drawLine(rect().bottomLeft() - QPoint(0, invisibleMargin - 2), rect().bottomRight() - QPoint(0, invisibleMargin - 2));
    } else {
        p.setPen(QColor(QColor(40, 40, 40)));
        p.drawLine(rect().topLeft(), rect().topRight());
        p.drawLine(rect().topLeft() + QPoint(0,1), rect().topRight() + QPoint(0,1));
        p.drawLine(rect().topLeft() + QPoint(0,2), rect().topRight() + QPoint(0,2));
    }
}
