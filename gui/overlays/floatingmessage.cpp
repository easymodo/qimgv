#include "floatingmessage.h"
#include "ui_floatingmessage.h"

FloatingMessage::FloatingMessage(OverlayContainerWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::FloatingMessage)
{
    ui->setupUi(this);
    hideDelay = 700;

    visibilityTimer.setSingleShot(true);
    visibilityTimer.setInterval(hideDelay);

    setFadeEnabled(true);
    setFadeDuration(300);

    position = FloatingWidgetPosition::LEFT;

    iconLeftEdge.load(":/res/icons/message/left_edge32.png");
    iconRightEdge.load(":/res/icons/message/right_edge32.png");
    setIcon(FloatingMessageIcon::NO_ICON);

    this->setAccessibleName("FloatingMessage");
    connect(&visibilityTimer, SIGNAL(timeout()), this, SLOT(hide()));
}

FloatingMessage::~FloatingMessage() {
    delete ui;
}

void FloatingMessage::showMessage(QString text, FloatingWidgetPosition position, FloatingMessageIcon icon, int duration) {
    this->position = position;
    hideDelay = duration;
    //
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
            ui->iconLabel->setPixmap(iconLeftEdge);
            break;
        case FloatingMessageIcon::ICON_RIGHT_EDGE:
            ui->iconLabel->show();
            ui->iconLabel->setPixmap(iconRightEdge);
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
