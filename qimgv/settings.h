#pragma once

#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QStandardPaths>
#include <QDebug>
#include <QImageReader>
#include <QStringList>
#include <QColor>
#include <QPalette>
#include <QDir>
#include <QKeySequence>
#include <QMap>
#include <QFont>
#include <QVersionNumber>
#include <QThread>
#include "utils/script.h"
#include "themestore.h"

enum SortingMode {
    SORT_NAME,
    SORT_NAME_DESC,
    SORT_SIZE,
    SORT_SIZE_DESC,
    SORT_TIME,
    SORT_TIME_DESC
};

enum ImageFitMode {
    FIT_WINDOW,
    FIT_WIDTH,
    FIT_ORIGINAL,
    FIT_FREE
};

enum PanelPosition {
    PANEL_TOP,
    PANEL_BOTTOM,
    PANEL_LEFT,
    PANEL_RIGHT
};

enum ScalingFilter {
    QI_FILTER_NEAREST,
    QI_FILTER_BILINEAR,
    QI_FILTER_CV_BILINEAR_SHARPEN,
    QI_FILTER_CV_CUBIC,
    QI_FILTER_CV_CUBIC_SHARPEN
};

enum ZoomIndicatorMode {
    INDICATOR_DISABLED,
    INDICATOR_ENABLED,
    INDICATOR_AUTO
};

enum DefaultCropAction {
    ACTION_CROP,
    ACTION_CROP_SAVE
};

enum ImageFocusPoint {
    FOCUS_TOP,
    FOCUS_CENTER,
    FOCUS_CURSOR
};

enum ImageScrolling {
    SCROLL_NONE,
    SCROLL_BY_TRACKPAD,
    SCROLL_BY_TRACKPAD_AND_WHEEL
};

enum ViewMode {
    MODE_DOCUMENT,
    MODE_FOLDERVIEW
};

enum FolderEndAction {
    FOLDER_END_NO_ACTION,
    FOLDER_END_LOOP,
    FOLDER_END_GOTO_ADJACENT
};

enum FolderViewMode {
    FV_SIMPLE,
    FV_EXTENDED,
    FV_EXT_FOLDERS
};

enum ThumbPanelStyle {
    TH_PANEL_SIMPLE,
    TH_PANEL_EXTENDED
};

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* getInstance();
    ~Settings();
    QStringList supportedMimeTypes();
    QList<QByteArray> supportedFormats();
    QString supportedFormatsFilter();
    QString supportedFormatsRegex();
    int panelPreviewsSize();
    void setPanelPreviewsSize(int size);
    bool usePreloader();
    void setUsePreloader(bool mode);
    bool fullscreenMode();
    void setFullscreenMode(bool mode);
    ImageFitMode imageFitMode();
    void setImageFitMode(ImageFitMode mode);
    QRect windowGeometry();
    void setWindowGeometry(QRect geometry);
    bool playVideoSounds();
    void setPlayVideoSounds(bool mode);
    void setVolume(int vol);
    int volume();
    QString thumbnailCacheDir();
    QString mpvBinary();
    void setMpvBinary(QString path);
    PanelPosition panelPosition();
    void setPanelPosition(PanelPosition);
    bool loopSlideshow();
    void setLoopSlideshow(bool mode);
    void readShortcuts(QMap<QString, QString> &shortcuts);
    void saveShortcuts(const QMap<QString, QString> &shortcuts);
    bool panelEnabled();
    void setPanelEnabled(bool mode);
    int lastDisplay();
    void setLastDisplay(int display);
    bool squareThumbnails();
    void setSquareThumbnails(bool mode);
    bool transparencyGrid();
    void setTransparencyGrid(bool mode);
    bool enableSmoothScroll();
    void setEnableSmoothScroll(bool mode);
    bool useThumbnailCache();
    void setUseThumbnailCache(bool mode);
    QStringList savedPaths();
    void setSavedPaths(QStringList paths);
    QString tmpDir();
    int thumbnailerThreadCount();
    void setThumbnailerThreadCount(int count);
    bool smoothUpscaling();
    void setSmoothUpscaling(bool mode);
    void setExpandImage(bool mode);
    bool expandImage();
    ScalingFilter scalingFilter();
    void setScalingFilter(ScalingFilter mode);
    bool smoothAnimatedImages();
    void setSmoothAnimatedImages(bool mode);
    bool panelFullscreenOnly();
    void setPanelFullscreenOnly(bool mode);
    QVersionNumber lastVersion();
    void setLastVersion(QVersionNumber &ver);
    void setShowChangelogs(bool mode);
    bool showChangelogs();
    qreal backgroundOpacity();
    void setBackgroundOpacity(qreal value);
    bool blurBackground();
    void setBlurBackground(bool mode);
    void setSortingMode(SortingMode mode);
    SortingMode sortingMode();    
    void readScripts(QMap<QString, Script> &scripts);
    void saveScripts(const QMap<QString, Script> &scripts);
    int folderViewIconSize();
    void setFolderViewIconSize(int value);

    bool firstRun();
    void setFirstRun(bool mode);

    void sync();
    bool cursorAutohide();
    void setCursorAutohide(bool mode);

    bool infoBarFullscreen();
    void setInfoBarFullscreen(bool mode);
    bool infoBarWindowed();
    void setInfoBarWindowed(bool mode);

    bool windowTitleExtendedInfo();
    void setWindowTitleExtendedInfo(bool mode);

    bool maximizedWindow();
    void setMaximizedWindow(bool mode);

    bool keepFitMode();
    void setKeepFitMode(bool mode);

    int expandLimit();
    void setExpandLimit(int value);

    qreal zoomStep();
    void setZoomStep(qreal value);
    int JPEGSaveQuality();
    void setJPEGSaveQuality(int value);
    bool useOpenGL();
    void setUseOpenGL(bool mode);
    void setZoomIndicatorMode(ZoomIndicatorMode mode);
    ZoomIndicatorMode zoomIndicatorMode();
    void setFocusPointIn1to1Mode(ImageFocusPoint mode);
    ImageFocusPoint focusPointIn1to1Mode();
    void setDefaultCropAction(DefaultCropAction mode);
    DefaultCropAction defaultCropAction();
    bool placesPanel();
    void setPlacesPanel(bool mode);

    QStringList bookmarks();
    void setBookmarks(QStringList paths);
    bool placesPanelBookmarksExpanded();
    void setPlacesPanelBookmarksExpanded(bool mode);
    bool placesPanelTreeExpanded();
    void setPlacesPanelTreeExpanded(bool mode);

    void setSlideshowInterval(int ms);
    int slideshowInterval();

    ImageScrolling imageScrolling();
    void setImageScrolling(ImageScrolling mode);

    int placesPanelWidth();
    void setPlacesPanelWidth(int width);

    ViewMode defaultViewMode();
    void setDefaultViewMode(ViewMode mode);

    FolderEndAction folderEndAction();
    void setFolderEndAction(FolderEndAction mode);

    const ColorScheme& colorScheme();
    void setColorScheme(ColorScheme scheme);
    void setColorTid(int tid);

    bool videoPlayback();
    void setVideoPlayback(bool mode);

    bool useSystemColorScheme();
    void setUseSystemColorScheme(bool mode);

    void loadStylesheet();

    bool showSaveOverlay();
    void setShowSaveOverlay(bool mode);
    bool confirmDelete();
    void setConfirmDelete(bool mode);
    bool confirmTrash();
    void setConfirmTrash(bool mode);

    FolderViewMode folderViewMode();
    void setFolderViewMode(FolderViewMode mode);

    const QMultiMap<QByteArray, QByteArray> videoFormats() const;

    bool printLandscape();
    void setPrintLandscape(bool mode);
    bool printPdfDefault();
    void setPrintPdfDefault(bool mode);
    bool printColor();
    void setPrintColor(bool mode);
    bool printFitToPage();
    void setPrintFitToPage(bool mode);
    QString lastPrinter();
    void setLastPrinter(QString name);
    bool unloadThumbs();
    void setUnloadThumbs(bool mode);
    ThumbPanelStyle thumbPanelStyle();
    void setThumbPanelStyle(ThumbPanelStyle mode);

    bool jxlAnimation();
    void setJxlAnimation(bool mode);
    bool absoluteZoomStep();
    void setAbsoluteZoomStep(bool mode);
    bool autoResizeWindow();
    void setAutoResizeWindow(bool mode);
    int autoResizeLimit();
    void setAutoResizeLimit(int percent);

    bool panelPinned();
    void setPanelPinned(bool mode);
    int memoryAllocationLimit();
    void setMemoryAllocationLimit(int limitMB);
    bool panelCenterSelection();
    void setPanelCenterSelection(bool mode);
    QString language();
    void setLanguage(QString lang);

private:
    explicit Settings(QObject *parent = nullptr);
    QSettings *settingsConf, *stateConf, *themeConf;
    QDir *mTmpDir, *mThumbCacheDir, *mConfDir;
    ColorScheme mColorScheme;
    QMultiMap<QByteArray, QByteArray> mVideoFormatsMap; // [mimetype, format]
    void loadTheme();
    void saveTheme();
    void createColorVariants();

    void setupCache();
    void fillVideoFormats();
signals:
    void settingsChanged();

public slots:
    void sendChangeNotification();

};

extern Settings *settings;
