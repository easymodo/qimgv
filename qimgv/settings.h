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
#include <QVersionNumber>
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

enum PanelHPosition {
    PANEL_TOP,
    PANEL_BOTTOM
};

enum PanelVPosition {
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

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* getInstance();
    ~Settings();
    QStringList supportedMimeTypes();
    QList<QByteArray> supportedFormats();
    QString supportedFormatsString();
    QString supportedFormatsRegex();
    int mainPanelSize();
    void setMainPanelSize(int size);
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
    bool showThumbnailLabels();
    void setShowThumbnailLabels(bool mode);
    PanelHPosition panelPosition();
    void setPanelPosition(PanelHPosition);

    bool infiniteScrolling();
    void setInfiniteScrolling(bool mode);
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

    bool shuffleEnabled();
    void setShuffleEnabled(bool mode);

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

    const ColorScheme& colorScheme();
    void setColorScheme(ColorScheme scheme);

    bool videoPlayback();
    void setVideoPlayback(bool mode);

    bool useSystemColorScheme();
    void setUseSystemColorScheme(bool mode);

    void loadStylesheet();

    bool showFolders();
    void setShowFolders(bool mode);
    bool showSaveOverlay();
    void setShowSaveOverlay(bool mode);
private:
    explicit Settings(QObject *parent = nullptr);
    const unsigned int mainPanelSizeDefault = 230;
    QSettings *settingsConf, *stateConf, *themeConf;
    QDir *mTmpDir, *mThumbCacheDir, *mConfDir;
    ColorScheme mColorScheme;
    void loadTheme();
    void saveTheme();
    void createColorVariants();

    void setupCache();
signals:
    void settingsChanged();

public slots:
    void sendChangeNotification();

};

extern Settings *settings;
