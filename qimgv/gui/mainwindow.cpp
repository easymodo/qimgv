#include "mainwindow.h"

#include <QFileInfo>

// TODO: nuke this and rewrite

MW::MW(QWidget *parent)
    : FloatingWidgetContainer(parent),
      currentDisplay(0),
      mActiveViewer(nullptr),
      mSplitMode(SPLIT_NONE),
      mSplitFocus(0),
      maximized(false),
      activeSidePanel(SIDEPANEL_NONE),
      copyOverlay(nullptr),
      saveOverlay{nullptr, nullptr},
      renameOverlay(nullptr),
      infoBarFullscreen(nullptr),
      imageInfoOverlay(nullptr),
      imageInfoOverlaySecondary(nullptr),
      floatingMessage(nullptr),
      cropPanel(nullptr),
      cropOverlay(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    layout.setContentsMargins(0,0,0,0);
    layout.setSpacing(0);

    setMinimumSize(10,10);

    // do not steal focus when clicked
    // this is just a container. accept key events only
    // via passthrough from child widgets
    setFocusPolicy(Qt::NoFocus);

    this->setLayout(&layout);

    setWindowTitle(QCoreApplication::applicationName() + " " +
                   QCoreApplication::applicationVersion());

    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    this->setAccessibleName("mainwindow");
    windowGeometryChangeTimer.setSingleShot(true);
    windowGeometryChangeTimer.setInterval(30);
    setupUi();

    connect(settings, &Settings::settingsChanged, this, &MW::readSettings);
    connect(&windowGeometryChangeTimer, &QTimer::timeout, this, &MW::onWindowGeometryChanged);
    connect(this, &MW::fullscreenStateChanged, this, &MW::adaptToWindowState);

    readSettings();
    currentDisplay = settings->lastDisplay();
    maximized = settings->maximizedWindow();
    restoreWindowGeometry();
}

/*                                                             |--[ImageViewer]
 *                        |--[DocumentWidget]--[ViewerWidget]--|
 * [MW]--[CentralWidget]--|                                    |--[VideoPlayer]
 *                        |--[FolderView]
 *
 *  (not counting floating widgets)
 *  ViewerWidget exists for input handling reasons (correct overlay hover handling)
 */
void MW::setupUi() {
    viewerWidget.reset(new ViewerWidget(this));
    viewerWidgetSecondary.reset(new ViewerWidget(this));
    mActiveViewer = viewerWidget.get();
    infoBarWindowed.reset(new InfoBarProxy(this));
    docWidget.reset(new DocumentWidget(viewerWidget, viewerWidgetSecondary, infoBarWindowed));
    folderView.reset(new FolderViewProxy(this));
    connect(folderView.get(), &FolderViewProxy::sortingSelected, this, &MW::sortingSelected);
    connect(folderView.get(), &FolderViewProxy::directorySelected, this, &MW::opened);
    connect(folderView.get(), &FolderViewProxy::copyUrlsRequested, this, &MW::copyUrlsRequested);
    connect(folderView.get(), &FolderViewProxy::moveUrlsRequested, this, &MW::moveUrlsRequested);
    connect(folderView.get(), &FolderViewProxy::showFoldersChanged, this, &MW::showFoldersChanged);

    centralWidget.reset(new CentralWidget(docWidget, folderView, this));
    layout.addWidget(centralWidget.get());
    controlsOverlay = new ControlsOverlay(docWidget.get());
    infoBarFullscreen = new FullscreenInfoOverlayProxy(viewerWidget.get());
    sidePanel = new SidePanel(this);
    layout.addWidget(sidePanel);
    imageInfoOverlay = new ImageInfoOverlayProxy(viewerWidget.get());
    imageInfoOverlaySecondary = new ImageInfoOverlayProxy(viewerWidgetSecondary.get());
    floatingMessage = new FloatingMessageProxy(viewerWidget.get()); // todo: use additional one for folderview?
    // a scale request belongs to whichever pane holds the focus frame,
    // not to a fixed one
    for(auto *v : {viewerWidget.get(), viewerWidgetSecondary.get()}) {
        connect(v, &ViewerWidget::scalingRequested, this, [this, v](QSize size, ScalingFilter filter) {
            if(v == activeViewer())
                emit scalingRequested(size, filter);
            else
                emit scalingRequestedInactive(size, filter);
        });
        connect(v, &ViewerWidget::draggedOut, this, qOverload<>(&MW::draggedOut));
        connect(v, &ViewerWidget::playbackFinished, this, &MW::playbackFinished);
    }
    connect(viewerWidget.get(), &ViewerWidget::showScriptSettings, this, &MW::showScriptSettings);
    connect(viewerWidgetSecondary.get(), &ViewerWidget::showScriptSettings, this, &MW::showScriptSettings);
    connect(this, &MW::setLoopPlayback,  viewerWidget.get(), &ViewerWidget::setLoopPlayback);
    connect(this, &MW::setLoopPlayback,  viewerWidgetSecondary.get(), &ViewerWidget::setLoopPlayback);
    // scroll sync between the two panes (while Ctrl is held)
    connect(viewerWidget.get(), &ViewerWidget::scrolled, this, [this](int dx, int dy, bool smooth) {
        syncScroll(viewerWidget.get(), dx, dy, smooth);
    });
    connect(viewerWidgetSecondary.get(), &ViewerWidget::scrolled, this, [this](int dx, int dy, bool smooth) {
        syncScroll(viewerWidgetSecondary.get(), dx, dy, smooth);
    });
    setViewerActionsConnected(viewerWidget.get(), true);
}

// everything the user can aim at a single pane goes to the focused viewer only
void MW::setViewerActionsConnected(ViewerWidget *w, bool connected) {
    if(!w)
        return;
    struct Link {
        void (MW::*from)();
        void (ViewerWidget::*to)();
    };
    static const Link links[] = {
        { &MW::zoomIn,             &ViewerWidget::zoomIn },
        { &MW::zoomOut,            &ViewerWidget::zoomOut },
        { &MW::zoomInCursor,       &ViewerWidget::zoomInCursor },
        { &MW::zoomOutCursor,      &ViewerWidget::zoomOutCursor },
        { &MW::scrollUp,           &ViewerWidget::scrollUp },
        { &MW::scrollDown,         &ViewerWidget::scrollDown },
        { &MW::scrollLeft,         &ViewerWidget::scrollLeft },
        { &MW::scrollRight,        &ViewerWidget::scrollRight },
        { &MW::pauseVideo,         &ViewerWidget::pauseResumePlayback },
        { &MW::stopPlayback,       &ViewerWidget::stopPlayback },
        { &MW::seekVideoForward,   &ViewerWidget::seekForward },
        { &MW::seekVideoBackward,  &ViewerWidget::seekBackward },
        { &MW::frameStep,          &ViewerWidget::frameStep },
        { &MW::frameStepBack,      &ViewerWidget::frameStepBack },
        { &MW::toggleMute,         &ViewerWidget::toggleMute },
        { &MW::volumeUp,           &ViewerWidget::volumeUp },
        { &MW::volumeDown,         &ViewerWidget::volumeDown },
        { &MW::toggleTransparencyGrid, &ViewerWidget::toggleTransparencyGrid },
    };
    for(const auto &link : links) {
        if(connected)
            connect(this, link.from, w, link.to);
        else
            disconnect(this, link.from, w, link.to);
    }
}

ViewerWidget *MW::activeViewer() {
    return mActiveViewer;
}

ViewerWidget *MW::inactiveViewer() {
    return (mActiveViewer == viewerWidget.get()) ? viewerWidgetSecondary.get()
                                                 : viewerWidget.get();
}

ImageInfoOverlayProxy *MW::activeInfoOverlay() {
    return splitFocusIndex() ? imageInfoOverlaySecondary : imageInfoOverlay;
}

ImageInfoOverlayProxy *MW::inactiveInfoOverlay() {
    return splitFocusIndex() ? imageInfoOverlay : imageInfoOverlaySecondary;
}

void MW::updateActiveViewer() {
    ViewerWidget *target = (mSplitMode != SPLIT_NONE && mSplitFocus == 1)
                            ? viewerWidgetSecondary.get() : viewerWidget.get();
    if(target == mActiveViewer)
        return;
    setViewerActionsConnected(mActiveViewer, false);
    mActiveViewer = target;
    setViewerActionsConnected(mActiveViewer, true);
    if(currentViewMode() == MODE_DOCUMENT)
        mActiveViewer->setFocus();
}

SplitViewMode MW::splitViewMode() {
    return mSplitMode;
}

int MW::splitFocusIndex() {
    return (mSplitMode == SPLIT_NONE) ? 0 : mSplitFocus;
}

void MW::setSplitViewMode(SplitViewMode mode) {
    if(mSplitMode == mode)
        return;
    mSplitMode = mode;
    if(mode == SPLIT_NONE)
        mSplitFocus = 0;
    updateActiveViewer();
    docWidget->setSplitViewMode(mode);
    docWidget->setSplitFocus(mSplitFocus);
    if(mode == SPLIT_NONE) {
        viewerWidgetSecondary->closeImage();
        imageInfoOverlaySecondary->hide();
    } else {
        viewerWidgetSecondary->setInteractionEnabled(viewerWidget->interactionEnabled());
        // the info panel is open in both panes or in neither
        if(!imageInfoOverlay->isHidden())
            imageInfoOverlaySecondary->show();
    }
}

void MW::toggleSplitFocus() {
    if(mSplitMode == SPLIT_NONE)
        return;
    mSplitFocus = mSplitFocus ? 0 : 1;
    updateActiveViewer();
    docWidget->setSplitFocus(mSplitFocus);
    emit splitFocusToggled();
}

/* Mirrors a scroll from one pane onto the other one while Shift is held.
 * The delta is rescaled by the zoom ratio, so the two images travel over
 * the same amount of source pixels.
 */
void MW::syncScroll(ViewerWidget *source, int dx, int dy, bool smooth) {
    if(mSplitMode == SPLIT_NONE)
        return;
    if(!(QGuiApplication::keyboardModifiers() & Qt::ShiftModifier))
        return;
    ViewerWidget *target = (source == viewerWidget.get()) ? viewerWidgetSecondary.get()
                                                          : viewerWidget.get();
    float sourceScale = source->currentScale();
    if(sourceScale <= 0.0f)
        return;
    float ratio = target->currentScale() / sourceScale;
    target->scrollRelative(qRound(dx * ratio), qRound(dy * ratio), smooth);
}

void MW::showImageInactive(std::unique_ptr<QPixmap> pixmap) {
    inactiveViewer()->showImage(std::move(pixmap));
}

void MW::showAnimationInactive(std::shared_ptr<QMovie> movie) {
    inactiveViewer()->showAnimation(movie);
}

void MW::showVideoInactive(QString file) {
    inactiveViewer()->showVideo(file);
}

void MW::closeImageInactive() {
    inactiveViewer()->closeImage();
}

void MW::onScalingFinishedInactive(std::unique_ptr<QPixmap> scaled) {
    inactiveViewer()->onScalingFinished(std::move(scaled));
}

void MW::setExifInfoInactive(QVector<QPair<QString, QString>> info) {
    inactiveInfoOverlay()->setExifInfo(info);
}

void MW::setupFullUi() {
    setupCropPanel();
    docWidget->allowPanelInit();
    docWidget->setupMainPanel();
    infoBarWindowed->init();
    infoBarFullscreen->init();
}

void MW::setupCropPanel() {
    if(cropPanel)
        return;
    cropOverlay = new CropOverlay(viewerWidget.get());
    cropPanel = new CropPanel(cropOverlay, this);
    connect(cropPanel, &CropPanel::cancel, this, &MW::hideCropPanel);
    connect(cropPanel, &CropPanel::crop,   this, &MW::hideCropPanel);
    connect(cropPanel, &CropPanel::crop,   this, &MW::cropRequested);
    connect(cropPanel, &CropPanel::cropAndSave, this, &MW::hideCropPanel);
    connect(cropPanel, &CropPanel::cropAndSave, this, &MW::cropAndSaveRequested);
}

void MW::setupCopyOverlay() {
    copyOverlay = new CopyOverlay(viewerWidget.get());
    connect(copyOverlay, &CopyOverlay::copyRequested, this, &MW::copyRequested);
    connect(copyOverlay, &CopyOverlay::moveRequested, this, &MW::moveRequested);
    copyOverlay->setCurrentDirectory(info.directoryPath);
}

void MW::setupSaveOverlay(int pane) {
    auto viewer = pane ? viewerWidgetSecondary.get() : viewerWidget.get();
    saveOverlay[pane] = new SaveConfirmOverlay(viewer);
    connect(saveOverlay[pane], &SaveConfirmOverlay::saveClicked,    this, &MW::saveRequested);
    connect(saveOverlay[pane], &SaveConfirmOverlay::saveAsClicked,  this, &MW::saveAsClicked);
    connect(saveOverlay[pane], &SaveConfirmOverlay::discardClicked, this, &MW::discardEditsRequested);
}

void MW::setupRenameOverlay() {
    renameOverlay = new RenameOverlay(this);
    renameOverlay->setName(info.fileName);
    connect(renameOverlay, &RenameOverlay::renameRequested, this, &MW::renameRequested);
}

void MW::toggleFolderView() {
    hideCropPanel();
    if(copyOverlay)
        copyOverlay->hide();
    if(renameOverlay)
        renameOverlay->hide();
    docWidget->hideFloatingPanel();
    imageInfoOverlay->hide();
    imageInfoOverlaySecondary->hide();
    centralWidget->toggleViewMode();
    onInfoUpdated();
}

void MW::enableFolderView() {
    hideCropPanel();
    if(copyOverlay)
        copyOverlay->hide();
    if(renameOverlay)
        renameOverlay->hide();
    docWidget->hideFloatingPanel();
    imageInfoOverlay->hide();
    imageInfoOverlaySecondary->hide();
    centralWidget->showFolderView();
    onInfoUpdated();
}

void MW::enableDocumentView() {
    centralWidget->showDocumentView();
    onInfoUpdated();
}

ViewMode MW::currentViewMode() {
    return centralWidget->currentViewMode();
}

void MW::fitWindow() {
    if(activeViewer()->interactionEnabled()) {
        activeViewer()->fitWindow();
    } else {
        showMessage("Zoom temporary disabled");
    }
}

void MW::fitWidth() {
    if(activeViewer()->interactionEnabled()) {
        activeViewer()->fitWidth();
    } else {
        showMessage("Zoom temporary disabled");
    }
}

void MW::fitOriginal() {
    if(activeViewer()->interactionEnabled()) {
        activeViewer()->fitOriginal();
    } else {
        showMessage("Zoom temporary disabled");
    }
}

void MW::fitWindowStretch() {
    if(activeViewer()->interactionEnabled()) {
        activeViewer()->fitWindowStretch();
    } else {
        showMessage("Zoom temporary disabled");
    }
}

// switch between 1:1 and Fit All
// TODO: move to viewerWidget?
void MW::switchFitMode() {
    if(activeViewer()->fitMode() == FIT_WINDOW)
        activeViewer()->setFitMode(FIT_ORIGINAL);
    else
        activeViewer()->setFitMode(FIT_WINDOW);
}

void MW::closeImage() {
    info.fileName = "";
    info.filePath = "";
    activeViewer()->closeImage();
}

// todo: fix flicker somehow
// ideally it should change img & resize in one go
void MW::preShowResize(QSize sz) {
    auto screens = qApp->screens();
    if(this->windowState() != Qt::WindowNoState || !screens.count() || screens.count() <= currentDisplay)
        return;
    int decorationSize = frameGeometry().height() - height();
    float maxSzMulti = settings->autoResizeLimit() / 100.f;
    QRect availableGeom = screens.at(currentDisplay)->availableGeometry();
    QSize maxSz = availableGeom.size() * maxSzMulti;
    maxSz.setHeight(maxSz.height() - decorationSize);
    if(!sz.isEmpty()) {
        if(sz.width() > maxSz.width() || sz.height() > maxSz.height())
            sz.scale(maxSz, Qt::KeepAspectRatio);
    } else {
        sz = maxSz;
    }
    QRect newGeom(0,0, sz.width(), sz.height());
    newGeom.moveCenter(availableGeom.center());
    newGeom.translate(0, decorationSize / 2);

    if(this->isVisible())
        setGeometry(newGeom);
    else // setGeometry wont work on hidden windows, so we just save for it to be restored later
        settings->setWindowGeometry(newGeom);
    qApp->processEvents(); // not needed anymore with patched qt?
}

void MW::showImage(std::unique_ptr<QPixmap> pixmap) {
    if(settings->autoResizeWindow())
        preShowResize(pixmap->size());
    activeViewer()->showImage(std::move(pixmap));
    updateCropPanelData();
}

void MW::showAnimation(std::shared_ptr<QMovie> movie) {
    if(settings->autoResizeWindow())
        preShowResize(movie->frameRect().size());
    activeViewer()->showAnimation(movie);
    updateCropPanelData();
}

void MW::showVideo(QString file) {
    if(settings->autoResizeWindow())
        preShowResize(QSize()); // tmp. find a way to get this though mpv BEFORE playback
    activeViewer()->showVideo(file);
}

void MW::showContextMenu() {
    viewerWidget->showContextMenu();
}

void MW::onSortingChanged(SortingMode mode) {
    folderView.get()->onSortingChanged(mode);
    if(centralWidget.get()->currentViewMode() == ViewMode::MODE_DOCUMENT) {
        switch(mode) {
            case SortingMode::SORT_NAME:      showMessage("Sorting: By Name");              break;
            case SortingMode::SORT_NAME_DESC: showMessage("Sorting: By Name (desc.)");      break;
            case SortingMode::SORT_TIME:      showMessage("Sorting: By Time");              break;
            case SortingMode::SORT_TIME_DESC: showMessage("Sorting: By Time (desc.)");      break;
            case SortingMode::SORT_SIZE:      showMessage("Sorting: By File Size");         break;
            case SortingMode::SORT_SIZE_DESC: showMessage("Sorting: By File Size (desc.)"); break;
        }
    }
}

void MW::setDirectoryPath(QString path) {
    //closeImage();
    info.directoryPath = path;
    info.directoryName = path.split("/").last();
    folderView->setDirectoryPath(path);
    onInfoUpdated();
    if(copyOverlay)
        copyOverlay->setCurrentDirectory(path);
}

void MW::toggleLockZoom() {
    activeViewer()->toggleLockZoom();
    if(activeViewer()->lockZoomEnabled())
        showMessage("Zoom lock: ON");
    else
        showMessage("Zoom lock: OFF");
    onInfoUpdated();
}

void MW::toggleLockView() {
    activeViewer()->toggleLockView();
    if(activeViewer()->lockViewEnabled())
        showMessage("View lock: ON");
    else
        showMessage("View lock: OFF");
    onInfoUpdated();
}

void MW::toggleFullscreenInfoBar() {
    if(!this->isFullScreen())
        return;
    showInfoBarFullscreen = !showInfoBarFullscreen;
    if(showInfoBarFullscreen)
        infoBarFullscreen->showWhenReady();
    else
        infoBarFullscreen->hide();
}

void MW::toggleImageInfoOverlay() {
    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW)
        return;
    // both panes at once, so their tags can be compared side by side
    bool show = imageInfoOverlay->isHidden();
    show ? imageInfoOverlay->show() : imageInfoOverlay->hide();
    if(show && mSplitMode != SPLIT_NONE)
        imageInfoOverlaySecondary->show();
    else
        imageInfoOverlaySecondary->hide();
}

void MW::toggleRenameOverlay(QString currentName) {
    if(!renameOverlay)
        setupRenameOverlay();
    if(renameOverlay->isHidden()) {
        renameOverlay->setBackdropEnabled((centralWidget->currentViewMode() == MODE_FOLDERVIEW));
        renameOverlay->setName(currentName);
        renameOverlay->show();
    } else {
        renameOverlay->hide();
    }
}

void MW::toggleScalingFilter() {
    ScalingFilter configuredFilter = settings->scalingFilter();
    if(activeViewer()->scalingFilter() == configuredFilter) {
        setFilterNearest();
    }
    else {
        setFilter(configuredFilter);
    }
}

void MW::setFilterNearest() {
    showMessage("Filter: nearest", 600);
    viewerWidget->setFilterNearest();
    viewerWidgetSecondary->setFilterNearest();
}

void MW::setFilterBilinear() {
    showMessage("Filter: bilinear", 600);
    viewerWidget->setFilterBilinear();
    viewerWidgetSecondary->setFilterBilinear();
}

void MW::setFilter(ScalingFilter filter) {
    QString filterName;
    switch (filter) {
        case QI_FILTER_NEAREST:
            filterName = "nearest";
            break;
        case ScalingFilter::QI_FILTER_BILINEAR:
            filterName = "bilinear";
            break;
        case QI_FILTER_CV_BILINEAR_SHARPEN:
            filterName = "bilinear + sharpen";
            break;
        case QI_FILTER_CV_CUBIC:
            filterName = "bicubic";
            break;
        case QI_FILTER_CV_CUBIC_SHARPEN:
            filterName = "bicubic + sharpen";
            break;
        default:
            filterName = "configured " + QString::number(static_cast<int>(filter));
            break;
    }
    showMessage("Filter " + filterName, 600);
    viewerWidget->setScalingFilter(filter);
    viewerWidgetSecondary->setScalingFilter(filter);
}

bool MW::isCropPanelActive() {
    return (activeSidePanel == SIDEPANEL_CROP);
}

void MW::onScalingFinished(std::unique_ptr<QPixmap> scaled) {
    activeViewer()->onScalingFinished(std::move(scaled));
}

void MW::saveWindowGeometry() {
    if(this->windowState() == Qt::WindowNoState)
        settings->setWindowGeometry(geometry());
    settings->setMaximizedWindow(maximized);
}

// does not apply fullscreen; window size / maximized state only
void MW::restoreWindowGeometry() {
    this->setGeometry(settings->windowGeometry());
    if(settings->maximizedWindow())
        this->setWindowState(Qt::WindowMaximized);
    updateCurrentDisplay();
}

void MW::updateCurrentDisplay() {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    currentDisplay = desktopWidget.screenNumber(this);
#else
    auto screens = qApp->screens();
    currentDisplay = screens.indexOf(this->window()->screen());
#endif
}

void MW::onWindowGeometryChanged() {
    saveWindowGeometry();
    updateCurrentDisplay();
}

void MW::saveCurrentDisplay() {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    settings->setLastDisplay(desktopWidget.screenNumber(this));
#else
    settings->setLastDisplay(qApp->screens().indexOf(this->window()->screen()));
#endif
}

//#############################################################
//######################### EVENTS ############################
//#############################################################

void MW::mouseMoveEvent(QMouseEvent *event) {
    event->ignore();
}

bool MW::event(QEvent *event) {
    // only save maximized state if we are already visible
    // this filter out out events while the window is still being set up
    if(event->type() == QEvent::WindowStateChange && this->isVisible() && !this->isFullScreen())
        maximized = isMaximized();
    if(event->type() == QEvent::Move || event->type() == QEvent::Resize)
        windowGeometryChangeTimer.start();
    return QWidget::event(event);
}

// hook up to actionManager
void MW::keyPressEvent(QKeyEvent *event) {
    event->accept();
    actionManager->processEvent(event);
}

/* Qt eats Tab for focus navigation before it ever reaches keyPressEvent.
 * Nothing in the viewer area is meant to be reached that way, and often no
 * widget holds the focus at all, so let the shortcut through in those cases.
 * Input fields (crop panel, rename overlay) keep the usual behaviour.
 */
bool MW::focusNextPrevChild(bool next) {
    auto focused = qApp->focusWidget();
    if(!focused || focused == viewerWidget.get() || focused == viewerWidgetSecondary.get() ||
       viewerWidget->isAncestorOf(focused) || viewerWidgetSecondary->isAncestorOf(focused))
    {
        return false;
    }
    return FloatingWidgetContainer::focusNextPrevChild(next);
}

void MW::wheelEvent(QWheelEvent *event) {
    event->accept();
    actionManager->processEvent(event);
}

void MW::mousePressEvent(QMouseEvent *event) {
    event->accept();
    actionManager->processEvent(event);
}

void MW::mouseReleaseEvent(QMouseEvent *event) {
    event->accept();
    actionManager->processEvent(event);
}

void MW::mouseDoubleClickEvent(QMouseEvent *event) {
    event->accept();
    QMouseEvent *fakePressEvent = new QMouseEvent(
        QEvent::MouseButtonPress,
        event->pos(),
        event->button(),
        event->buttons(),
        event->modifiers()
    );
    actionManager->processEvent(fakePressEvent);
    actionManager->processEvent(event);
}

void MW::close() {
    saveWindowGeometry();
    saveCurrentDisplay();
    // try to close window sooner
    // since qt6.3 QWidget::close() no longer works on hidden windows (bug?)
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
    this->hide();
#endif
    if(copyOverlay)
        copyOverlay->saveSettings();
    QWidget::close();
}

void MW::closeEvent(QCloseEvent *event) {
    // catch the close event when user presses X on the window itself
    event->accept();
    actionManager->invokeAction("exit");
}

void MW::dragEnterEvent(QDragEnterEvent *e) {
    if(e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MW::dropEvent(QDropEvent *event) {
    emit droppedIn(event->mimeData(), event->source());
}

void MW::resizeEvent(QResizeEvent *event) {
    if(activeSidePanel == SIDEPANEL_CROP) {
        cropOverlay->setImageScale(viewerWidget->currentScale());
        cropOverlay->setImageDrawRect(viewerWidget->imageRect());
    }
    FloatingWidgetContainer::resizeEvent(event);
}

void MW::showDefault() {
    if(!this->isVisible()) {
        if(settings->fullscreenMode())
            showFullScreen();
        else
            showWindowed();
    }
}

void MW::showSaveDialog(QString filePath) {
    QString newFilePath = getSaveFileName(filePath);
    if(!newFilePath.isEmpty())
        emit saveAsRequested(newFilePath);
}

QString MW::getSaveFileName(QString filePath) {
    docWidget->hideFloatingPanel();
    QStringList filters;
    // generate filter for writable images
    // todo: some may need to be blacklisted
    auto writerFormats = QImageWriter::supportedImageFormats();
    if(writerFormats.contains("jpg"))  filters.append("JPEG (*.jpg *.jpeg *jpe *jfif)");
    if(writerFormats.contains("png"))  filters.append("PNG (*.png)");
    if(writerFormats.contains("webp")) filters.append("WebP (*.webp)");
    // may not work..
    if(writerFormats.contains("jp2"))  filters.append("JPEG 2000 (*.jp2 *.j2k *.jpf *.jpx *.jpm *.jpgx)");
    if(writerFormats.contains("jxl"))  filters.append("JPEG-XL (*.jxl)");
    if(writerFormats.contains("avif")) filters.append("AVIF (*.avif *.avifs)");
    if(writerFormats.contains("tif"))  filters.append("TIFF (*.tif *.tiff)");
    if(writerFormats.contains("bmp"))  filters.append("BMP (*.bmp)");
#ifdef _WIN32
    if(writerFormats.contains("ico"))  filters.append("Icon Files (*.ico)");
#endif
    if(writerFormats.contains("ppm"))  filters.append("PPM (*.ppm)");
    if(writerFormats.contains("xbm"))  filters.append("XBM (*.xbm)");
    if(writerFormats.contains("xpm"))  filters.append("XPM (*.xpm)");
    if(writerFormats.contains("dds"))  filters.append("DDS (*.dds)");
    if(writerFormats.contains("wbmp")) filters.append("WBMP (*.wbmp)");
    // add everything else from imagewriter
    for(auto fmt : writerFormats) {
        if(filters.filter(fmt).isEmpty())
            filters.append(fmt.toUpper() + " (*." + fmt + ")");
    }
    QString filterString = filters.join(";; ");

    // find matching filter for the current image
    QString selectedFilter = "JPEG (*.jpg *.jpeg *jpe *jfif)";
    QFileInfo fi(filePath);
    for(auto filter : filters) {
        if(filter.contains(fi.suffix().toLower())) {
            selectedFilter = filter;
            break;
        }
    }
    QString newFilePath = QFileDialog::getSaveFileName(this, tr("Save File as..."), filePath, filterString, &selectedFilter);
    return newFilePath;
}

void MW::showOpenDialog(QString path) {
    docWidget->hideFloatingPanel();

    QFileDialog dialog(this);
    QStringList imageFilter;
    imageFilter.append(settings->supportedFormatsFilter());
    imageFilter.append("All Files (*)");
    dialog.setDirectory(path);
    dialog.setNameFilters(imageFilter);
    dialog.setWindowTitle("Open image");
    dialog.setWindowModality(Qt::ApplicationModal);
    connect(&dialog, &QFileDialog::fileSelected, this, &MW::opened);
    dialog.exec();
}

void MW::showResizeDialog(QSize initialSize) {
    ResizeDialog dialog(initialSize, this);
    connect(&dialog, &ResizeDialog::sizeSelected, this, &MW::resizeRequested);
    dialog.exec();
}

DialogResult MW::fileReplaceDialog(QString src, QString dst, FileReplaceMode mode, bool multiple) {
    FileReplaceDialog dialog(this);
    dialog.setModal(true);
    dialog.setSource(src);
    dialog.setDestination(dst);
    dialog.setMode(mode);
    dialog.setMulti(multiple);

    dialog.exec();

    return dialog.getResult();
}

void MW::showSettings() {
    docWidget->hideFloatingPanel();
    SettingsDialog settingsDialog(this);
    settingsDialog.exec();
}

void MW::showScriptSettings() {
    docWidget->hideFloatingPanel();
    SettingsDialog settingsDialog(this);
    settingsDialog.switchToPage(4);
    settingsDialog.exec();
}

void MW::triggerFullScreen() {
    if(!isFullScreen()) {
        showFullScreen();
    } else {
        showWindowed();
    }
}

void MW::showFullScreen() {
    //do not save immediately on application start
    if(!isHidden())
        saveWindowGeometry();
    auto screens = qApp->screens();
    // todo: why check the screen again?
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    int _currentDisplay = desktopWidget.screenNumber(this);
#else
    int _currentDisplay = screens.indexOf(this->window()->screen());
#endif
    //move to target screen
    if(screens.count() > currentDisplay && currentDisplay != _currentDisplay) {
        this->move(screens.at(currentDisplay)->geometry().x(),
                   screens.at(currentDisplay)->geometry().y());
    }
    QWidget::showFullScreen();
    // try to repaint sooner
    qApp->processEvents();
    emit fullscreenStateChanged(true);
}

void MW::showWindowed() {
    if(isFullScreen())
        QWidget::showNormal();
    restoreWindowGeometry();
    QWidget::show();
    // try to repaint sooner
    qApp->processEvents();
    emit fullscreenStateChanged(false);
}

void MW::updateCropPanelData() {
    if(cropPanel && activeSidePanel == SIDEPANEL_CROP) {
        cropPanel->setImageRealSize(viewerWidget->sourceSize());
        cropOverlay->setImageDrawRect(viewerWidget->imageRect());
        cropOverlay->setImageScale(viewerWidget->currentScale());
        cropOverlay->setImageRealSize(viewerWidget->sourceSize());
        cropOverlay->setMcuSize(cropMcuSize);
    }
}

// The MCU grid a crop selection should snap to for the crop to stay
// lossless, pushed in by Core (which knows the file and what's already
// been done to it). An empty size means don't snap.
void MW::setCropMcuSize(QSize size) {
    cropMcuSize = size;
    if(cropOverlay && activeSidePanel == SIDEPANEL_CROP)
        cropOverlay->setMcuSize(size);
}

/* The save prompt is an action on the file the shortcuts operate on, so it
 * only ever shows up on the pane holding the focus frame.
 */
void MW::showSaveOverlay() {
    hideSaveOverlay();
    if(!settings->showSaveOverlay())
        return;
    int pane = splitFocusIndex();
    if(!saveOverlay[pane])
        setupSaveOverlay(pane);
    saveOverlay[pane]->show();
}

void MW::hideSaveOverlay() {
    for(auto *overlay : saveOverlay)
        if(overlay)
            overlay->hide();
}

void MW::showChangelogWindow() {
    changelogWindow->show();
}

void MW::showChangelogWindow(QString text) {
    changelogWindow->setText(text);
    changelogWindow->show();
}

void MW::triggerCropPanel() {
    if(activeSidePanel != SIDEPANEL_CROP) {
        showCropPanel();
    } else {
        hideCropPanel();
    }
}

void MW::showCropPanel() {
    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW)
        return;

    if(activeSidePanel != SIDEPANEL_CROP) {
        docWidget->hideFloatingPanel();
        sidePanel->setWidget(cropPanel);
        sidePanel->show();
        cropOverlay->show();
        activeSidePanel = SIDEPANEL_CROP;
        // reset & lock zoom so CropOverlay won't go crazy
        viewerWidget->fitWindow();
        setInteractionEnabled(false);
        // feed the panel current image info
        updateCropPanelData();
    }
}

void MW::setInteractionEnabled(bool mode) {
    docWidget->setInteractionEnabled(mode);
    viewerWidget->setInteractionEnabled(mode);
    viewerWidgetSecondary->setInteractionEnabled(mode);
}

void MW::hideCropPanel() {
    sidePanel->hide();
    if(activeSidePanel == SIDEPANEL_CROP) {
        cropOverlay->hide();
        setInteractionEnabled(true);
    }
    activeSidePanel = SIDEPANEL_NONE;
}

void MW::triggerCopyOverlay() {
    if(!viewerWidget->isDisplaying())
        return;
    if(!copyOverlay)
        setupCopyOverlay();

    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW)
        return;
    if(copyOverlay->operationMode() == OVERLAY_COPY) {
        copyOverlay->isHidden() ? copyOverlay->show() : copyOverlay->hide();
    } else {
        copyOverlay->setDialogMode(OVERLAY_COPY);
        copyOverlay->show();
    }
}

void MW::triggerMoveOverlay() {
    if(!viewerWidget->isDisplaying())
        return;
    if(!copyOverlay)
        setupCopyOverlay();

    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW)
        return;
    if(copyOverlay->operationMode() == OVERLAY_MOVE) {
        copyOverlay->isHidden() ? copyOverlay->show() : copyOverlay->hide();
    } else {
        copyOverlay->setDialogMode(OVERLAY_MOVE);
        copyOverlay->show();
    }
}

// quit fullscreen or exit the program
void MW::closeFullScreenOrExit() {
    if(this->isFullScreen()) {
        this->showWindowed();
    } else {
        actionManager->invokeAction("exit");
    }
}

// todo: this is crap, use shared state object
void MW::setCurrentInfo(int _index, int _fileCount, QString _filePath, QString _fileName, QString _groupNameSuffix, QSize _imageSize, qint64 _fileSize, bool slideshow, bool shuffle, bool edited) {
    info.index = _index;
    info.fileCount = _fileCount;
    info.fileName = _fileName;
    info.groupNameSuffix = _groupNameSuffix;
    info.filePath = _filePath;
    info.imageSize = _imageSize;
    info.fileSize = _fileSize;
    info.slideshow = slideshow;
    info.shuffle = shuffle;
    info.edited = edited;
    onInfoUpdated();
}

// todo: nuke and rewrite
void MW::onInfoUpdated() {
    QString posString;
    if(info.fileCount)
        posString = "[ " + QString::number(info.index + 1) + "/" + QString::number(info.fileCount) + " ]";
    QString resString;
    if(info.imageSize.width())
        resString = QString::number(info.imageSize.width()) + " x " + QString::number(info.imageSize.height());
    QString sizeString;
    if(info.fileSize)
        sizeString = this->locale().formattedDataSize(info.fileSize, 1);

    if(renameOverlay)
        renameOverlay->setName(info.fileName);

    QString windowTitle;
    if(centralWidget->currentViewMode() == MODE_FOLDERVIEW) {
        windowTitle = tr("Folder view");
        infoBarFullscreen->setInfo("", tr("No file opened."), "");
        infoBarWindowed->setInfo("", tr("No file opened."), "");
    } else if(info.fileName.isEmpty()) {
        windowTitle = qApp->applicationName();
        infoBarFullscreen->setInfo("", tr("No file opened."), "");
        infoBarWindowed->setInfo("", tr("No file opened."), "");
    } else {
        windowTitle = info.fileName + info.groupNameSuffix;
        if(settings->windowTitleExtendedInfo()) {
            windowTitle.prepend(posString + "  ");
            if(!resString.isEmpty())
                windowTitle.append("  -  " + resString);
            if(!sizeString.isEmpty())
                windowTitle.append("  -  " + sizeString);
        }

        // toggleable states
        QString states;
        if(info.slideshow)
            states.append(" [slideshow]");
        if(info.shuffle)
            states.append(" [shuffle]");
        if(activeViewer()->lockZoomEnabled())
            states.append(" [zoom lock]");
        if(activeViewer()->lockViewEnabled())
            states.append(" [view lock]");

        if(!settings->infoBarWindowed() && !states.isEmpty())
            windowTitle.append(" -" + states);
        if(info.edited)
            windowTitle.prepend("* ");

        QString barName = info.fileName + info.groupNameSuffix + (info.edited ? "  *" : "");
        infoBarFullscreen->setInfo(posString, barName, resString + "  " + sizeString);
        infoBarWindowed->setInfo(posString, barName, resString + "  " + sizeString + " " + states);
    }
    setWindowTitle(windowTitle);
}

// TODO!!! buffer this in mw
void MW::setExifInfo(QVector<QPair<QString, QString>> info) {
    if(imageInfoOverlay)
        activeInfoOverlay()->setExifInfo(info);
}

std::shared_ptr<FolderViewProxy> MW::getFolderView() {
    return folderView;
}

std::shared_ptr<ThumbnailStripProxy> MW::getThumbnailPanel() {
    return docWidget->thumbPanel();
}

// todo: this is crap
void MW::showMessageDirectory(QString dirName) {
    floatingMessage->showMessage(dirName, FloatingMessageIcon::ICON_DIRECTORY, 1700);
}

void MW::showMessageDirectoryEnd() {
    // TODO replace with something nicer (integrate with click overlay?)
    //floatingMessage->showMessage("", FloatingWidgetPosition::RIGHT, FloatingMessageIcon::ICON_RIGHT_EDGE, 400);
}

void MW::showMessageDirectoryStart() {
    // TODO replace with something nicer (integrate with click overlay?)
    //floatingMessage->showMessage("", FloatingWidgetPosition::LEFT, FloatingMessageIcon::ICON_LEFT_EDGE, 400);
}

void MW::showMessageFitWindow() {
    floatingMessage->showMessage(tr("Fit Window"), FloatingMessageIcon::NO_ICON, 350);
}

void MW::showMessageFitWidth() {
    floatingMessage->showMessage(tr("Fit Width"), FloatingMessageIcon::NO_ICON, 350);
}

void MW::showMessageFitOriginal() {
    floatingMessage->showMessage(tr("Fit 1:1"), FloatingMessageIcon::NO_ICON, 350);
}

void MW::showMessage(QString text) {
    floatingMessage->showMessage(text,  FloatingMessageIcon::NO_ICON, 1500);
}

void MW::showMessage(QString text, int duration) {
    floatingMessage->showMessage(text, FloatingMessageIcon::NO_ICON, duration);
}

void MW::showMessageSuccess(QString text) {
    floatingMessage->showMessage(text,  FloatingMessageIcon::ICON_SUCCESS, 1500);
}

void MW::showWarning(QString text) {
    floatingMessage->showMessage(text,  FloatingMessageIcon::ICON_WARNING, 1500);
}

void MW::showError(QString text) {
    floatingMessage->showMessage(text,  FloatingMessageIcon::ICON_ERROR, 2800);
}

bool MW::showConfirmation(QString title, QString msg) {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setModal(true);
    if(msgBox.exec() == QMessageBox::Yes)
        return true;
    else
        return false;
}

void MW::readSettings() {
    showInfoBarFullscreen = settings->infoBarFullscreen();
    showInfoBarWindowed = settings->infoBarWindowed();
    adaptToWindowState();
}

// todo: remove/rename?
void MW::applyWindowedBackground() {
#ifdef USE_KDE_BLUR
    QWindow* window = this->windowHandle();
    if(window) {
        if(settings->backgroundOpacity() == 1.0)
            KWindowEffects::enableBlurBehind(window, false);
        else
            KWindowEffects::enableBlurBehind(window, settings->blurBackground());
    }
#endif
}

void MW::applyFullscreenBackground() {
#ifdef USE_KDE_BLUR
    QWindow* window = this->windowHandle();
    if(window)
        KWindowEffects::enableBlurBehind(window, false);
#endif
}

// changes ui elements according to fullscreen state
void MW::adaptToWindowState() {
    docWidget->hideFloatingPanel();
    if(isFullScreen()) { //-------------------------------------- fullscreen ---
        applyFullscreenBackground();
        infoBarWindowed->hide();

        if(showInfoBarFullscreen)
            infoBarFullscreen->showWhenReady();
        else
            infoBarFullscreen->hide();    

        auto pos = settings->panelPosition();
        if(!settings->panelEnabled() || pos == PANEL_BOTTOM || pos == PANEL_LEFT)
            controlsOverlay->show();
        else
            controlsOverlay->hide();
    } else { //------------------------------------------------------ window ---
        applyWindowedBackground();
        infoBarFullscreen->hide();

        if(showInfoBarWindowed)
            infoBarWindowed->show();
        else
            infoBarWindowed->hide();

        controlsOverlay->hide();
    }
    folderView->onFullscreenModeChanged(isFullScreen());
    docWidget->onFullscreenModeChanged(isFullScreen());
    viewerWidget->onFullscreenModeChanged(isFullScreen());
    viewerWidgetSecondary->onFullscreenModeChanged(isFullScreen());
}

void MW::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
    FloatingWidgetContainer::paintEvent(event);
}

void MW::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    docWidget->hideFloatingPanel(true);
}

// block native tab-switching so we can use it in shortcuts
//bool MW::focusNextPrevChild(bool) {
//    return false;
//}
