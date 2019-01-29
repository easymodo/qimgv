#include "folderview.h"
#include "ui_folderview.h"

FolderView::FolderView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FolderView)
{
    //this->setAttribute(Qt::WA_NoMousePropagation, true);
    ui->setupUi(this);

    ui->openButton->setAction("open");
    ui->settingsButton->setAction("openSettings");
    ui->closeButton->setAction("exit");
    ui->docViewButton->setAction("documentView");

    int min = ui->thumbnailGrid->THUMBNAIL_SIZE_MIN;
    int max = ui->thumbnailGrid->THUMBNAIL_SIZE_MAX;
    int step = ui->thumbnailGrid->ZOOM_STEP;

    ui->zoomSlider->setMinimum(min / step);
    ui->zoomSlider->setMaximum(max / step);
    ui->zoomSlider->setSingleStep(1);
    ui->zoomSlider->setPageStep(1);

    connect(ui->thumbnailGrid, SIGNAL(thumbnailPressed(int)),
            this, SIGNAL(thumbnailPressed(int)));
    connect(ui->thumbnailGrid, SIGNAL(thumbnailRequested(QList<int>, int)),
            this, SIGNAL(thumbnailRequested(QList<int>, int)));

    connect(ui->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(onZoomSliderValueChanged(int)));
    connect(ui->thumbnailGrid, SIGNAL(thumbnailSizeChanged(int)), this, SLOT(onThumbnailSizeChanged(int)));
    connect(ui->thumbnailGrid, SIGNAL(showLabelsChanged(bool)), this, SLOT(onShowLabelsChanged(bool)));
    connect(ui->showLabelsButton, SIGNAL(toggled(bool)), this, SLOT(onShowLabelsButtonToggled(bool)));
    ui->thumbnailGrid->setThumbnailSize(settings->folderViewIconSize());
    ui->thumbnailGrid->setShowLabels(settings->showThumbnailLabels());
}

void FolderView::onShowLabelsChanged(bool mode) {
    ui->showLabelsButton->setChecked(mode);
    settings->setShowThumbnailLabels(mode);
}

void FolderView::onShowLabelsButtonToggled(bool mode) {
    ui->thumbnailGrid->setShowLabels(mode);
}

void FolderView::onThumbnailSizeChanged(int newSize) {
    ui->zoomSlider->setValue(newSize / ui->thumbnailGrid->ZOOM_STEP);
    settings->setFolderViewIconSize(newSize);
}

void FolderView::onZoomSliderValueChanged(int value) {
    ui->thumbnailGrid->setThumbnailSize(value * ui->thumbnailGrid->ZOOM_STEP);
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

void FolderView::setCloseButtonEnabled(bool mode) {
    ui->closeButton->setHidden(!mode);
}

void FolderView::focusInEvent(QFocusEvent *event) {
    Q_UNUSED(event)
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

void FolderView::addItem() {
    ui->thumbnailGrid->addItem();
}

void FolderView::insertItem(int index) {
    ui->thumbnailGrid->insertItem(index);
}

void FolderView::removeItem(int index) {
    ui->thumbnailGrid->removeItem(index);
}

// prevent passthrough to parent
void FolderView::wheelEvent(QWheelEvent *event) {
    event->accept();
}

void FolderView::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
