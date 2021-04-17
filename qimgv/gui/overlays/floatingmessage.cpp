#include "floatingmessage.h"
#include "ui_floatingmessage.h"

FloatingMessage::FloatingMessage(FloatingWidgetContainer *parent) :
    OverlayWidget(parent),
    preferredPosition(FloatingWidgetPosition::BOTTOM),
    ui(new Ui::FloatingMessage)
{
    ui->setupUi(this);
    hideDelay = 700;

    visibilityTimer.setSingleShot(true);
    visibilityTimer.setInterval(hideDelay);

    setFadeEnabled(true);
    setFadeDuration(300);

    setIcon(FloatingMessageIcon::NO_ICON);

    this->setAccessibleName("FloatingMessage");
    connect(&visibilityTimer, &QTimer::timeout, this, &FloatingMessage::hideAnimated);

    readSettings();

    connect(settings, &Settings::settingsChanged, this, &FloatingMessage::readSettings);

    if(parent)
        setContainerSize(parent->size());
}

FloatingMessage::~FloatingMessage() {
    delete ui;
}

void FloatingMessage::readSettings() {
    /*
    // don't interfere with the main panel
    if(settings->panelEnabled() && settings->panelPosition() == PanelHPosition::PANEL_BOTTOM) {
        preferredPosition = FloatingWidgetPosition::TOP;
    } else {
        preferredPosition = FloatingWidgetPosition::BOTTOM;
    }
    */
}

void FloatingMessage::showMessage(QString text, FloatingWidgetPosition position, FloatingMessageIcon icon, int duration) {
    setPosition(position);
    doShowMessage(text, icon, duration);
}

void FloatingMessage::showMessage(QString text, FloatingMessageIcon icon, int duration) {
    setPosition(preferredPosition);
    doShowMessage(text, icon, duration);
}

void FloatingMessage::doShowMessage(QString text, FloatingMessageIcon icon, int duration) {
    hideDelay = duration;
    setIcon(icon);
    setText(text);
    show();
}

void FloatingMessage::setText(QString text) {
    ui->textLabel->setText(text);
    text.isEmpty()?ui->textLabel->hide():ui->textLabel->show();
    recalculateGeometry();
    update();
}

void FloatingMessage::setIcon(FloatingMessageIcon icon) {
    switch (icon) {
        case FloatingMessageIcon::NO_ICON:
        case FloatingMessageIcon::ICON_WARNING:
        case FloatingMessageIcon::ICON_ERROR:
            //ui->iconLabel->setIconPath(":/res/icons/common/notifications/error16.png");
            ui->iconLabel->hide();
            break;
        case FloatingMessageIcon::ICON_DIRECTORY:
            ui->iconLabel->show();
            ui->iconLabel->setIconPath(":/res/icons/common/buttons/panel/folder16.png");
            break;
        case FloatingMessageIcon::ICON_LEFT_EDGE:
            ui->iconLabel->show();
            ui->iconLabel->setIconPath(":/res/icons/common/notifications/dir_start20.png");
            break;
        case FloatingMessageIcon::ICON_RIGHT_EDGE:
            ui->iconLabel->show();
            ui->iconLabel->setIconPath(":/res/icons/common/notifications/dir_end20.png");
            break;
        case FloatingMessageIcon::ICON_SUCCESS:
            ui->iconLabel->show();
            ui->iconLabel->setIconPath(":/res/icons/common/notifications/success16.png");
            break;
    }
}

void FloatingMessage::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED (event)
}

// "blink" the widget; show then fade out immediately
void FloatingMessage::show() {
    visibilityTimer.stop();
    OverlayWidget::show();
    // fade out after delay
    visibilityTimer.setInterval(hideDelay);
    visibilityTimer.start();
}
