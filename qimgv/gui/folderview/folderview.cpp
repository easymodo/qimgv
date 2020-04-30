#include "folderview.h"
#include "ui_folderview.h"

FolderView::FolderView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FolderView)
{
    ui->setupUi(this);

    // ------- filesystem view --------
    QString style = "font: %1pt;";
    style = style.arg(QApplication::font().pointSize());
    ui->dirTreeView->setStyleSheet(style);

    dirModel = new FileSystemModelCustom(this);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    ui->dirTreeView->setModel(dirModel);

    QHeaderView* header = ui->dirTreeView->header();
    header->hideSection(1); // size
    header->hideSection(2); // type
    header->hideSection(3); // mod date

#ifdef _WIN32
    dirModel->setRootPath("");
#else
    dirModel->setRootPath(QDir::homePath());
    QModelIndex idx = dirModel->index(dirModel->rootPath());
    ui->dirTreeView->setRootIndex(idx);
#endif
    // -------------------------------

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
    ui->togglePlacesPanelButton->setCheckable(true);
    ui->togglePlacesPanelButton->setIconPath(":res/icons/buttons/panel/toggle-panel20.png");

    ui->newBookmarkButton->setIconPath(":res/icons/buttons/add-new12.png");
    ui->homeButton->setIconPath(":res/icons/buttons/home12.png");
    ui->rootButton->setIconPath(":res/icons/buttons/root12.png");

    int min = ui->thumbnailGrid->THUMBNAIL_SIZE_MIN;
    int max = ui->thumbnailGrid->THUMBNAIL_SIZE_MAX;
    int step = ui->thumbnailGrid->ZOOM_STEP;

    ui->zoomSlider->setMinimum(min / step);
    ui->zoomSlider->setMaximum(max / step);
    ui->zoomSlider->setSingleStep(1);
    ui->zoomSlider->setPageStep(1);

    ui->splitter->setStretchFactor(1, 50);

    connect(ui->thumbnailGrid, &FolderGridView::itemSelected,     this, &FolderView::itemSelected);
    connect(ui->thumbnailGrid, &FolderGridView::thumbnailsRequested,  this, &FolderView::thumbnailsRequested);
    connect(ui->thumbnailGrid, &FolderGridView::thumbnailSizeChanged, this, &FolderView::onThumbnailSizeChanged);
    connect(ui->thumbnailGrid, &FolderGridView::showLabelsChanged,    this, &FolderView::onShowLabelsChanged);
    connect(ui->thumbnailGrid, &FolderGridView::draggedOut,     this, &FolderView::draggedOut);

    connect(ui->bookmarksWidget, &BookmarksWidget::bookmarkClicked, this, &FolderView::onBookmarkClicked);

    connect(ui->newBookmarkButton, &IconButton::clicked, this, &FolderView::newBookmark);
    connect(ui->homeButton, &IconButton::clicked, this, &FolderView::onHomeBtn);
    connect(ui->rootButton, &IconButton::clicked, this, &FolderView::onRootBtn);

    connect(ui->zoomSlider, &QSlider::valueChanged, this, &FolderView::onZoomSliderValueChanged);
    connect(ui->sortingComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &FolderView::onSortingSelected);
    connect(ui->showLabelsButton, &ActionButton::toggled, this, &FolderView::onShowLabelsButtonToggled);
    connect(ui->togglePlacesPanelButton, &ActionButton::toggled, this, &FolderView::onPlacesPanelButtonChecked);

    connect(ui->dirTreeView, &TreeViewCustom::droppedIn, this, &FolderView::onDroppedInByIndex);
    connect(ui->bookmarksWidget, &BookmarksWidget::droppedIn, this, &FolderView::onDroppedIn);

    ui->sortingComboBox->setItemDelegate(new QStyledItemDelegate(ui->sortingComboBox));
    ui->sortingComboBox->view()->setTextElideMode(Qt::ElideNone);

    connect(ui->splitter, &QSplitter::splitterMoved, this, &FolderView::onSplitterMoved);

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
    ui->togglePlacesPanelButton->setChecked(settings->placesPanel());

    setPlacesPanel(settings->placesPanel());
    ui->bookmarksWidget->setVisible(settings->placesPanelBookmarksExpanded());
    ui->dirTreeView->setVisible(settings->placesPanelTreeExpanded());

    QList<int> sizes;
    sizes << settings->placesPanelWidth() << 1;
    ui->splitter->setSizes(sizes);

    onSortingChanged(settings->sortingMode());
}

void FolderView::onSplitterMoved() {
    settings->setPlacesPanelWidth(ui->placesPanel->width());
}

void FolderView::onPlacesPanelButtonChecked(bool mode) {
    setPlacesPanel(mode);
    settings->setPlacesPanel(mode);
}

void FolderView::setPlacesPanel(bool mode) {
    if(width() >= 600)
        ui->placesPanel->setVisible(mode);
}

void FolderView::toggleBookmarks() {
    if(ui->bookmarksWidget->isVisible())
        ui->bookmarksWidget->hide();
    else
        ui->bookmarksWidget->show();
    settings->setPlacesPanelBookmarksExpanded(ui->bookmarksWidget->isVisible());
}

void FolderView::toggleFilesystemView() {
    if(ui->dirTreeView->isVisible())
        ui->dirTreeView->hide();
    else
        ui->dirTreeView->show();
    settings->setPlacesPanelTreeExpanded(ui->dirTreeView->isVisible());
}

void FolderView::onDroppedInByIndex(QList<QUrl> urls, QModelIndex index) {
    emit moveUrlsRequested(urls, dirModel->filePath(index));
}

void FolderView::onDroppedIn(QList<QUrl> urls, QString dirPath) {
    emit moveUrlsRequested(urls, dirPath);
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

void FolderView::onHomeBtn() {
    emit directorySelected(QDir::homePath());
}

void FolderView::onRootBtn() {
    emit directorySelected("/");
}

void FolderView::setDirectoryPath(QString path) {
#ifdef __linux
    if(path.startsWith(QDir::homePath())) {
        if(dirModel->rootPath() != QDir::homePath()) {
            dirModel->setRootPath(QDir::homePath());
            QModelIndex idx = dirModel->index(dirModel->rootPath());
            ui->dirTreeView->setRootIndex(idx);
        }
    } else {
        dirModel->setRootPath("/");
        QModelIndex idx = dirModel->index(dirModel->rootPath());
        ui->dirTreeView->setRootIndex(idx);
    }
#endif
    if(ui->dirTreeView->currentIndex().data() == path)
        return;

    ui->bookmarksWidget->onPathChanged(path);

    QModelIndex targetIndex = dirModel->index(path);
    bool keepExpand = ui->dirTreeView->isExpanded(targetIndex);
    bool collapse = !ui->dirTreeView->isExpanded(ui->dirTreeView->currentIndex().parent());

    if(collapse)
        ui->dirTreeView->collapseAll();
    ui->dirTreeView->setCurrentIndex(targetIndex);

    if(keepExpand)
        ui->dirTreeView->expand(targetIndex);

    // ok, i'm done with this shit. none of the "solutions" work
    // just do scrollTo after a delay and hope that model is loaded by then
    // larger than ~150ms becomes too noticeable
    QTimer::singleShot(150, this, &FolderView::fsTreeScrollToCurrent);
}

void FolderView::fsTreeScrollToCurrent() {
    ui->dirTreeView->scrollTo(ui->dirTreeView->currentIndex());
}

void FolderView::onTreeViewClicked(QModelIndex index) {
    QString path = dirModel->fileInfo(index).absoluteFilePath();
    emit directorySelected(dirModel->fileInfo(index).absoluteFilePath());
}

void FolderView::onBookmarkClicked(QString dirPath) {
    emit directorySelected(dirPath);
}

void FolderView::newBookmark() {
    QString dirPath;

    QFileDialog dialog(this);
    dialog.setDirectory(QDir::homePath());
    dialog.setWindowTitle("Select directory");
    dialog.setWindowModality(Qt::ApplicationModal);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    dialog.setOption(QFileDialog::DontResolveSymlinks);
    connect(&dialog, &QFileDialog::fileSelected, ui->bookmarksWidget, &BookmarksWidget::addBookmark);
    dialog.exec();
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
    Q_UNUSED(event)
    if(!ui->togglePlacesPanelButton->isChecked())
        return;
    if(width() < 600) {
        if(ui->placesPanel->isVisible())
            ui->placesPanel->setVisible(false);
    } else {
        if(!ui->placesPanel->isVisible())
            ui->placesPanel->setVisible(true);
    }
}
