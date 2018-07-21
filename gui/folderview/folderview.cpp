#include "folderview.h"
#include "ui_folderview.h"

FolderView::FolderView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FolderView)
{
    //this->setAttribute(Qt::WA_NoMousePropagation, true);
    ui->setupUi(this);

    ui->openButton->setAction("open");
    ui->openButton->setPixmap(QPixmap(":/res/icons/buttons/open20.png"));
    ui->settingsButton->setAction("openSettings");
    ui->settingsButton->setPixmap(QPixmap(":/res/icons/buttons/settings20.png"));

    connect(ui->thumbnailGrid, SIGNAL(thumbnailPressed(int)),
            this, SIGNAL(thumbnailPressed(int)));
    connect(ui->thumbnailGrid, SIGNAL(thumbnailRequested(QList<int>, int)),
            this, SIGNAL(thumbnailRequested(QList<int>, int)));
}

FolderView::~FolderView() {
    delete ui;
}

// probably unneeded
void FolderView::show() {
    QWidget::show();
    ui->thumbnailGrid->setFocus();
}

// probably unneeded
void FolderView::hide() {
    QWidget::hide();
    ui->thumbnailGrid->clearFocus();
}

void FolderView::focusInEvent(QFocusEvent *event) {
    ui->thumbnailGrid->setFocus();
}

void FolderView::populate(int count) {
    ui->thumbnailGrid->populate(count);
}

void FolderView::setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) {
    ui->thumbnailGrid->setThumbnail(pos, thumb);
}

void FolderView::selectIndex(int index) {
    ui->thumbnailGrid->selectIndex(index);
}

void FolderView::setDirectoryPath(QString path) {
    ui->directoryPathLabel->setText(path);
}

// prevent passthrough to parent
void FolderView::mouseReleaseEvent(QMouseEvent *event) {
}

// prevent passthrough to parent
void FolderView::wheelEvent(QWheelEvent *event) {
    event->accept();
}
