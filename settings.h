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

enum PanelPosition {
    BOTTOM,
    TOP,
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
    bool menuBarHidden();
    void setMenuBarHidden(bool mode);
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
    void setFfmpegExecutable(QString path);
    bool showThumbnailLabels();
    void setShowThumbnailLabels(bool mode);
    PanelPosition panelPosition();
    void setPanelPosition(PanelPosition);
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

private:
    explicit Settings(QObject *parent = 0);
    const int thumbnailSizeDefault = 160;
    QSettings s;
    QDir *tempDirectory;

signals:
    void settingsChanged();

public slots:
    void sendChangeNotification();

};

extern Settings *settings;

#endif // SETTINGS_H
