#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <QImageReader>
#include <QStringList>
#include <QColor>

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* getInstance();
    static void validate();
    QStringList supportedFormats();
    QString supportedFormatsString();
    int sortingMode();
    bool setSortingMode(int);

    bool useFastScale();
    void setUseFastScale(bool mode);
    QString lastDirectory();
    void setLastDirectory(QString path);
    unsigned int lastFilePosition();
    void setLastFilePosition(unsigned int pos);
    unsigned int thumbnailSize();
    void setThumbnailSize(unsigned int size);
    bool usePreloader();
    bool setUsePreloader(bool mode);
    QColor backgroundColor();
    void setBackgroundColor(QColor color);
    bool fullscreenMode();
    void setFullscreenMode(bool mode);
    bool menuBarHidden();
    void setMenuBarHidden(bool mode);
    int imageFitMode();
    void setImageFitMode(int mode);
    QByteArray windowGeometry();
    void setWindowGeometry(QByteArray geometry);
    bool reduceRamUsage();
    void setReduceRamUsage(bool mode);

private:
    explicit Settings(QObject *parent = 0);
    const int thumbnailSizeDefault = 120;
    QSettings s;

signals:
    void settingsChanged();

public slots:
    void sendChangeNotification();

};

extern Settings *globalSettings;

#endif // SETTINGS_H
