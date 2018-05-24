#include "changelogwindow.h"
#include "ui_changelogwindow.h"

ChangelogWindow::ChangelogWindow(OverlayContainerWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::ChangelogWindow)
{
    ui->setupUi(this);
    hide();
    setPosition(FloatingWidgetPosition::CENTER);
    connect(ui->closeButton, SIGNAL(pressed()), this, SLOT(hide()));
    connect(ui->shutUpButton, SIGNAL(pressed()), this, SLOT(hideAndShutUp()));
}

void ChangelogWindow::hideAndShutUp() {
    settings->setShowChangelogs(false);
    hide();
}

ChangelogWindow::~ChangelogWindow() {
    delete ui;
}

void ChangelogWindow::setText(QString text) {
    ui->textBrowser->setText(text);
}

void ChangelogWindow::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// move to base class?
void ChangelogWindow::wheelEvent(QWheelEvent *event) {
    event->accept();
}
