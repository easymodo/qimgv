#include "floatingmessage.h"
#include "ui_floatingmessage.h"

FloatingMessage::FloatingMessage(OverlayContainerWidget *parent) :
    FloatingWidget(parent),
    preferredPosition(FloatingWidgetPosition::BOTTOM),
    ui(new Ui::FloatingMessage)
{
    ui->setupUi(this);
    hideDelay = 700;

    visibilityTimer.setSingleShot(true);
    visibilityTimer.setInterval(hideDelay);

    setFadeEnabled(true);
    setFadeDuration(300);

    iconLeftEdge = QIcon(":/res/icons/message/dir_start20.png");
    iconRightEdge = QIcon(":/res/icons/message/dir_end20.png");
    iconSuccess = QIcon(":/res/icons/buttons/success16.png");
    setIcon(FloatingMessageIcon::NO_ICON);

    this->setAccessibleName("FloatingMessage");
    connect(&visibilityTimer, SIGNAL(timeout()), this, SLOT(hide()));

    readSettings();

    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));

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
            ui->iconLabel->hide();
            break;
        case FloatingMessageIcon::ICON_LEFT_EDGE:
            ui->iconLabel->show();
            ui->iconLabel->setPixmap(iconLeftEdge.pixmap(20,20));
            break;
        case FloatingMessageIcon::ICON_RIGHT_EDGE:
            ui->iconLabel->show();
            ui->iconLabel->setPixmap(iconRightEdge.pixmap(20,20));
            break;
        case FloatingMessageIcon::ICON_SUCCESS:
            ui->iconLabel->show();
            ui->iconLabel->setPixmap(iconSuccess.pixmap(16,16));
            break;
    }
}

void FloatingMessage::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED (event)
}

// "blink" the widget; show then fade out immediately
void FloatingMessage::show() {
    visibilityTimer.stop();
    FloatingWidget::show();
    // fade out after delay
    visibilityTimer.setInterval(hideDelay);
    visibilityTimer.start();
}
