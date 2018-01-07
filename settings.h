#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <QImageReader>
#include <QStringList>
#include <QColor>
#include <QDir>
#include <QKeySequence>
#include <QMap>
#include "components/actionmanager/actionmanager.h"

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

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* getInstance();
    static void validate();
    QStringList supportedMimeTypes();
    QString supportedFormatsString();
    bool useFastScale();
    void setUseFastScale(bool mode);
    QString lastDirectory();
    void setLastDirectory(QString path);
    unsigned int lastFilePosition();
    void setLastFilePosition(unsigned int pos);
    unsigned int mainPanelSize();
    void setMainPanelSize(unsigned int size);
    bool usePreloader();
    void setUsePreloader(bool mode);
    QColor backgroundColor();
    void setBackgroundColor(QColor color);
    QColor accentColor();
    void setAccentColor(QColor color);
    bool fullscreenMode();
    void setFullscreenMode(bool mode);
    ImageFitMode imageFitMode();
    void setImageFitMode(ImageFitMode mode);
    QRect windowGeometry();
    void setWindowGeometry(QRect geometry);
    bool reduceRamUsage();
    void setReduceRamUsage(bool mode);
    bool playVideos();
    void setPlayVideos(bool mode);
    bool playVideoSounds();
    void setPlayVideoSounds(bool mode);
    QString cacheDir();
    QString mpvBinary();
    void setMpvBinary(QString path);
    bool showThumbnailLabels();
    void setShowThumbnailLabels(bool mode);
    PanelHPosition panelPosition();
    void setPanelPosition(PanelHPosition);
    ~Settings();
    bool infiniteScrolling();
    void setInfiniteScrolling(bool mode);
    bool fullscreenTaskbarShown();
    void setFullscreenTaskbarShown(bool mode);
    QStringList supportedFormats();
    void readShortcuts();
    void saveShortcuts();
    bool panelEnabled();
    void setPanelEnabled(bool mode);
    int lastDisplay();
    void setLastDisplay(int display);
    bool mouseWrapping();
    void setMouseWrapping(bool mode);
    bool squareThumbnails();
    void setSquareThumbnails(bool mode);
    bool drawThumbnailSelectionBorder();
    void setDrawThumbnailSelectionBorder(bool mode);
    bool transparencyGrid();
    void setTransparencyGrid(bool mode);
    bool forceSmoothScroll();
    void setForceSmoothScroll(bool mode);
    bool useThumbnailCache();
    void setUseThumbnailCache(bool mode);
    QStringList savedPaths();
    void setSavedPaths(QStringList paths);
    QString tempDir();
    int thumbnailerThreadCount();
    void setThumbnailerThreadCount(int count);
    bool smoothUpscaling();
    void setSmoothUpscaling(bool mode);
    int maxZoomedResolution();
    void setMaxZoomedResolution(int value);
    int maximumZoom();
    void setMaximumZoom(int value);
    void setExpandImage(bool mode);
    bool expandImage();
    int scalingFilter();
    void setScalingFilter(int mode);
    bool smoothAnimatedImages();
    void setSmoothAnimatedImages(bool mode);

    bool panelFullscreenOnly();
    void setPanelFullscreenOnly(bool mode);
private:
    explicit Settings(QObject *parent = 0);
    const int mainPanelSizeDefault = 210;
    QSettings s;
    QDir *cacheDirectory, *thumbnailDirectory;

signals:
    void settingsChanged();

public slots:
    void sendChangeNotification();

};

extern Settings *settings;

#endif // SETTINGS_H
