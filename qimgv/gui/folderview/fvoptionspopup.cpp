#include "fvoptionspopup.h"
#include "ui_fvoptionspopup.h"

FVOptionsPopup::FVOptionsPopup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FVOptionsPopup)
{
    ui->setupUi(this);

#ifdef _WIN32
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#else
    setWindowFlags(Qt::Popup);
#endif
    setAttribute(Qt::WA_TranslucentBackground, true);
    //setAttribute(Qt::WA_NoMousePropagation, true);

    ui->viewSimpleButton->setText("Images only");
    ui->viewSimpleButton->setIconPath(":/res/icons/common/menuitem/folder16.png");

    ui->viewRegularButton->setText("Images & info");
    ui->viewRegularButton->setIconPath(":/res/icons/common/menuitem/folder16.png");

    ui->viewFoldersButton->setText("Images & Folders");
    ui->viewFoldersButton->setIconPath(":/res/icons/common/menuitem/folder16.png");

    // force resize
    this->adjustSize();

    readSettings();
    connect(settings, &Settings::settingsChanged, this, &FVOptionsPopup::readSettings);

    hide();
}

FVOptionsPopup::~FVOptionsPopup() {
    delete ui;
}

void FVOptionsPopup::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FVOptionsPopup::keyPressEvent(QKeyEvent *event) {
    quint32 nativeScanCode = event->nativeScanCode();
    QString key = actionManager->keyForNativeScancode(nativeScanCode);
    if(key == "Esc")
        hide();
    else
        actionManager->processEvent(event);
}

void FVOptionsPopup::readSettings() {

}

void FVOptionsPopup::showAt(QPoint pos) {
    QRect geom = geometry();
    geom.moveTopLeft(pos);
    setGeometry(geom);
    show();
}

void FVOptionsPopup::hideEvent(QHideEvent* event) {
    event->accept();
    emit dismissed();
}

/*void FVOptionsPopup::recalculateGeometry() {
    //setGeometry(0,0, containerSize().width(), containerSize().height());
    setGeometry(350, 34, width(), height());
}
*/
