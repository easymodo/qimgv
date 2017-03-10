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
#include "actionmanager.h"

enum PanelHPosition {
    TOP,
    BOTTOM
};

enum PanelVPosition {
    LEFT,
    RIGHT
};

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* getInstance();
    static void validate();
    QStringList supportedMimeTypes();
    QString supportedFormatsString();
    int sortingMode();
    void setSortingMode(int);
    bool useFastScale();
    void setUseFastScale(bool mode);
    QString lastDirectory();
    void setLastDirectory(QString path);
    unsigned int lastFilePosition();
    void setLastFilePosition(unsigned int pos);
    unsigned int thumbnailSize();
    void setThumbnailSize(unsigned int size);
    bool usePreloader();
    void setUsePreloader(bool mode);
    QColor backgroundColor();
    void setBackgroundColor(QColor color);
    QColor accentColor();
    void setAccentColor(QColor color);
    bool fullscreenMode();
    void setFullscreenMode(bool mode);
    int imageFitMode();
    void setImageFitMode(int mode);
    QRect windowGeometry();
    void setWindowGeometry(QRect geometry);
    bool reduceRamUsage();
    void setReduceRamUsage(bool mode);
    bool playVideos();
    void setPlayVideos(bool mode);
    bool playVideoSounds();
    void setPlayVideoSounds(bool mode);
    QString tempDir();
    QString ffmpegExecutable();
    void setFFmpegExecutable(QString path);
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
    bool setPanelEnabled(bool mode);
    int lastDisplay();
    int setLastDisplay(int display);
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

    PanelVPosition sidePanelPosition();
    void setSidePanelPosition(PanelVPosition pos);
    bool sidePanelEnabled();
    bool setSidePanelEnabled(bool mode);
private:
    explicit Settings(QObject *parent = 0);
    const int thumbnailSizeDefault = 190;
    QSettings s;
    QDir *tempDirectory;

signals:
    void settingsChanged();

public slots:
    void sendChangeNotification();

};

extern Settings *settings;

#endif // SETTINGS_H
