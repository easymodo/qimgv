#include "floatingmessage.h"
#include "ui_floatingmessage.h"

FloatingMessage::FloatingMessage(ContainerWidget *parent) :
    FloatingWidget(parent),
    duration(1200),
    ui(new Ui::FloatingMessage)
{
    ui->setupUi(this);
    opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(1.0f);
    this->setGraphicsEffect(opacityEffect);
    position = FloatingWidgetPosition::LEFT;

    iconLeftEdge.load(":/res/icons/message/left_edge32.png");
    iconRightEdge.load(":/res/icons/message/right_edge32.png");
    setIcon(FloatingMessageIcon::NO_ICON);

    fadeAnimation = new QPropertyAnimation(this, "opacity");
    fadeAnimation->setDuration(duration);
    fadeAnimation->setStartValue(1.0f);
    fadeAnimation->setEndValue(0.0f);
    fadeAnimation->setEasingCurve(QEasingCurve::InQuad);
    connect(fadeAnimation, SIGNAL(finished()), this, SLOT(hide()), Qt::UniqueConnection);
    this->setAccessibleName("FloatingMessage");
}

FloatingMessage::~FloatingMessage() {
    delete opacityEffect;
    delete fadeAnimation;
    delete ui;
}


void FloatingMessage::showMessage(QString text, FloatingWidgetPosition position, FloatingMessageIcon icon, int duration) {
    this->position = position;
    this->duration = duration;
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

float FloatingMessage::opacity() const {
    return opacityEffect->opacity();
}

void FloatingMessage::setOpacity(float opacity) {
    opacityEffect->setOpacity(opacity);
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

void FloatingMessage::show() {
    fadeAnimation->stop();
    QWidget::show();
    fadeAnimation->setDuration(duration);
    fadeAnimation->start(QPropertyAnimation::KeepWhenStopped);
}
