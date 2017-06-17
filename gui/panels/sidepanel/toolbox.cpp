#include "toolbox.h"

ToolBox::ToolBox(QWidget *parent) : QWidget(parent) {
    setAccessibleName("panelButtonsWidget");

    rotateRight = new ClickableLabel();
    rotateRight->setAccessibleName("panelButton");
    rotateRight->setPixmap(QPixmap(":/res/icons/buttons/rotate-right48.png"));
    rotateRight->setFixedSize(48,48);
    rotateLeft = new ClickableLabel();
    rotateLeft->setAccessibleName("panelButton");
    rotateLeft->setPixmap(QPixmap(":/res/icons/buttons/rotate-left48.png"));
    rotateLeft->setFixedSize(48,48);
    zoomFit = new ClickableLabel();
    zoomFit->setAccessibleName("panelButton");
    zoomFit->setPixmap(QPixmap(":/res/icons/buttons/zoom-fit48.png"));
    zoomFit->setFixedSize(48,48);
    zoomWidth = new ClickableLabel();
    zoomWidth->setAccessibleName("panelButton");
    zoomWidth->setPixmap(QPixmap(":/res/icons/buttons/zoom-width48.png"));
    zoomWidth->setFixedSize(48,48);
    zoomOriginal = new ClickableLabel();
    zoomOriginal->setAccessibleName("panelButton");
    zoomOriginal->setPixmap(QPixmap(":/res/icons/buttons/zoom-original48.png"));
    zoomOriginal->setFixedSize(48,48);
    zoomIn = new ClickableLabel();
    zoomIn->setAccessibleName("panelButton");
    zoomIn->setPixmap(QPixmap(":/res/icons/buttons/zoom-in48.png"));
    zoomIn->setFixedSize(48,48);
    zoomOut = new ClickableLabel();
    zoomOut->setAccessibleName("panelButton");
    zoomOut->setPixmap(QPixmap(":/res/icons/buttons/zoom-out48.png"));
    zoomOut->setFixedSize(48,48);

    buttonsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(0,0,0,0);
    buttonsLayout->addWidget(rotateRight);
    buttonsLayout->addWidget(rotateLeft);
    buttonsLayout->addWidget(zoomFit);
    buttonsLayout->addWidget(zoomWidth);
    buttonsLayout->addWidget(zoomOriginal);
    buttonsLayout->addWidget(zoomIn);
    buttonsLayout->addWidget(zoomOut);

    this->setLayout(buttonsLayout);
    resize(buttonsLayout->sizeHint());
    connect(rotateLeft, SIGNAL(clicked()), this, SIGNAL(rotateLeftClicked()));
    connect(rotateRight, SIGNAL(clicked()), this, SIGNAL(rotateRightClicked()));
    connect(zoomFit, SIGNAL(clicked()), this, SIGNAL(zoomFitClicked()));
    connect(zoomWidth, SIGNAL(clicked()), this, SIGNAL(zoomWidthClicked()));
    connect(zoomOriginal, SIGNAL(clicked()), this, SIGNAL(zoomOriginalClicked()));
    connect(zoomIn, SIGNAL(clicked()), this, SIGNAL(zoomInClicked()));
    connect(zoomOut, SIGNAL(clicked()), this, SIGNAL(zoomOutClicked()));
}

void ToolBox::readSettings() {

}

QSize ToolBox::sizeHint() {
    // TODO
    return size();
}
