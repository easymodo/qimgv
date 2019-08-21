#include "saveconfirmoverlay.h"
#include "ui_saveconfirmoverlay.h"

SaveConfirmOverlay::SaveConfirmOverlay(OverlayContainerWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::SaveConfirmOverlay)
{
    ui->setupUi(this);
    connect(ui->saveButton, SIGNAL(clicked()), this, SIGNAL(saveClicked()));
    connect(ui->saveAsButton, SIGNAL(clicked()), this, SIGNAL(saveAsClicked()));
    connect(ui->discardButton, SIGNAL(clicked()), this, SIGNAL(discardClicked()));
    this->setFocusPolicy(Qt::NoFocus);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));

    this->hide();
}

void SaveConfirmOverlay::readSettings() {
    // don't interfere with the main panel
    if(settings->panelEnabled() && settings->panelPosition() == PanelHPosition::PANEL_BOTTOM) {
        setPosition(FloatingWidgetPosition::TOPRIGHT);
    } else {
        setPosition(FloatingWidgetPosition::BOTTOMRIGHT);
    }
    update();
}

SaveConfirmOverlay::~SaveConfirmOverlay()
{
    delete ui;
}
