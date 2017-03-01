#include "toolbox.h"

ToolBox::ToolBox(QWidget *parent) : QWidget(parent) {
    setAccessibleName("panelButtonsWidget");

    rotateLeft = new ClickableLabel();
    rotateLeft->setAccessibleName("panelButton");
    rotateLeft->setPixmap(QPixmap(":/images/res/icons/rotate-left.png"));
    rotateLeft->setFixedSize(48,48);
    rotateRight = new ClickableLabel();
    rotateRight->setAccessibleName("panelButton");
    rotateRight->setPixmap(QPixmap(":/images/res/icons/rotate-right.png"));
    rotateRight->setFixedSize(48,48);

    buttonsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(0,0,0,0);
    buttonsLayout->addWidget(rotateLeft);
    buttonsLayout->addWidget(rotateRight);

    this->setLayout(buttonsLayout);
    resize(buttonsLayout->sizeHint());
    connect(rotateLeft, SIGNAL(clicked()), this, SIGNAL(rotateLeftClicked()));
    connect(rotateRight, SIGNAL(clicked()), this, SIGNAL(rotateRightClicked()));
}
