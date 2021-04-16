#include "changelogwindow.h"
#include "ui_changelogwindow.h"

ChangelogWindow::ChangelogWindow(FloatingWidgetContainer *parent) :
    OverlayWidget(parent),
    ui(new Ui::ChangelogWindow)
{
    ui->setupUi(this);
    hide();
    setPosition(FloatingWidgetPosition::CENTER);
    connect(ui->closeButton, &QPushButton::pressed,  this, &ChangelogWindow::hide);
    connect(ui->shutUpButton, &QPushButton::pressed, this, &ChangelogWindow::hideAndShutUp);

    if(parent)
        setContainerSize(parent->size());
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
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// move to base class?
void ChangelogWindow::wheelEvent(QWheelEvent *event) {
    event->accept();
}

void ChangelogWindow::keyPressEvent(QKeyEvent *event) {
    quint32 nativeScanCode = event->nativeScanCode();
    QString key = actionManager->keyForNativeScancode(nativeScanCode);
    if(key == "Esc") {
        event->accept();
        hide();
    }
}

void ChangelogWindow::show() {
    OverlayWidget::show();
    ui->closeButton->setFocus();
}

void ChangelogWindow::hide() {
    ui->closeButton->clearFocus();
    ui->shutUpButton->clearFocus();
    OverlayWidget::hide();
}
