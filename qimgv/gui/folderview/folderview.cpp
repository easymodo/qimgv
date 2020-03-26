#include "folderview.h"
#include "ui_folderview.h"

FolderView::FolderView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FolderView)
{
    ui->setupUi(this);
    mWrapper.reset(new DirectoryViewWrapper(this));

    dirModel = new FileSystemModelCustom(this);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    ui->dirTreeView->setModel(dirModel);

    // tmp
    ui->bookmarksListView->setModel(dirModel);

    QHeaderView* header = ui->dirTreeView->header();

    header->hideSection(1); // size
    header->hideSection(2); // type
    header->hideSection(3); // mod date

#ifndef _WIN32
    dirModel->setRootPath(QDir::homePath());
    QModelIndex idx = dirModel->index(dirModel->rootPath());
    ui->dirTreeView->setRootIndex(idx);

    ui->bookmarksListView->setRootIndex(idx);
#endif

    ui->openButton->setAction("open");
    ui->openButton->setIconPath(":res/icons/buttons/panel/open16.png");
    ui->settingsButton->setAction("openSettings");
    ui->settingsButton->setIconPath(":res/icons/buttons/panel/settings16.png");
    ui->exitButton->setAction("exit");
    ui->exitButton->setIconPath(":res/icons/buttons/panel/close16.png");
    ui->docViewButton->setAction("documentView");
    ui->docViewButton->setIconPath(":res/icons/buttons/panel/document-view16.png");
    ui->showLabelsButton->setCheckable(true);
    ui->showLabelsButton->setIconPath(":res/icons/buttons/panel/labels.png");

    int min = ui->thumbnailGrid->THUMBNAIL_SIZE_MIN;
    int max = ui->thumbnailGrid->THUMBNAIL_SIZE_MAX;
    int step = ui->thumbnailGrid->ZOOM_STEP;

    ui->zoomSlider->setMinimum(min / step);
    ui->zoomSlider->setMaximum(max / step);
    ui->zoomSlider->setSingleStep(1);
    ui->zoomSlider->setPageStep(1);

    connect(ui->thumbnailGrid, &FolderGridView::itemSelected,     this, &FolderView::itemSelected);
    connect(ui->thumbnailGrid, &FolderGridView::thumbnailsRequested,  this, &FolderView::thumbnailsRequested);
    connect(ui->thumbnailGrid, &FolderGridView::thumbnailSizeChanged, this, &FolderView::onThumbnailSizeChanged);
    connect(ui->thumbnailGrid, &FolderGridView::showLabelsChanged,    this, &FolderView::onShowLabelsChanged);
    connect(ui->thumbnailGrid, &FolderGridView::draggedOut,     this, &FolderView::draggedOut);

    connect(ui->zoomSlider, &QSlider::valueChanged, this, &FolderView::onZoomSliderValueChanged);
    connect(ui->sortingComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &FolderView::onSortingSelected);
    connect(ui->showLabelsButton, &ActionButton::toggled, this, &FolderView::onShowLabelsButtonToggled);

    connect(ui->dirTreeView, &TreeViewCustom::droppedIn, this, &FolderView::onDroppedIn);

    ui->sortingComboBox->setItemDelegate(new QStyledItemDelegate(ui->sortingComboBox));
    ui->sortingComboBox->view()->setTextElideMode(Qt::ElideNone);

    readSettings();

    QSizePolicy sp_retain = sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    setSizePolicy(sp_retain);
    connect(settings, &Settings::settingsChanged, this, &FolderView::readSettings);
    hide();
}

void FolderView::readSettings() {
    ui->thumbnailGrid->setThumbnailSize(settings->folderViewIconSize());
    ui->thumbnailGrid->setShowLabels(settings->showThumbnailLabels());
    onSortingChanged(settings->sortingMode());
}

void FolderView::onDroppedIn(QList<QUrl> urls, QModelIndex index) {
    emit moveUrlsRequested(urls, dirModel->filePath(index));
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

// changed by user via combobox
void FolderView::onSortingSelected(int mode) {
    emit sortingSelected(static_cast<SortingMode>(mode));
}

void FolderView::onSortingChanged(SortingMode mode) {
    ui->sortingComboBox->blockSignals(true);
    ui->sortingComboBox->setCurrentIndex(static_cast<int>(mode));
    ui->sortingComboBox->blockSignals(false);
}

FolderView::~FolderView() {
    delete ui;
}

std::shared_ptr<DirectoryViewWrapper> FolderView::wrapper() {
    return mWrapper;
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

void FolderView::onFullscreenModeChanged(bool mode) {
    ui->exitButton->setHidden(!mode);
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

int FolderView::selectedIndex() {
    return ui->thumbnailGrid->selectedIndex();
}

void FolderView::focusOn(int index) {
    ui->thumbnailGrid->focusOn(index);
}

void FolderView::setDirectoryPath(QString path) {
    //dirModel->setRootPath(path);
    //QModelIndex idx = dirModel->index(dirModel->rootPath());
    //ui->dirTreeView->setRootIndex(idx);

    QModelIndex index = dirModel->index(path);

    ui->directoryPathLabel->setText(path);
    dirModel->index(path);
    ui->dirTreeView->expand(index);
    ui->dirTreeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void FolderView::onTreeViewClicked(QModelIndex index) {
    QString path = dirModel->fileInfo(index).absoluteFilePath();
    emit directorySelected(dirModel->fileInfo(index).absoluteFilePath());
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

void FolderView::reloadItem(int index) {
    ui->thumbnailGrid->reloadItem(index);
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

void FolderView::resizeEvent(QResizeEvent *event) {

}
