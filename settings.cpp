#include "settings.h"

Settings *globalSettings=NULL;

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    tempDirectory = new QTemporaryDir();
}

Settings* Settings::getInstance() {
    if (!globalSettings) {
        globalSettings=new Settings();
    }
    validate();
    return globalSettings;
}

void Settings::validate() {
    if(globalSettings) {
        bool ok = true;
        if(globalSettings->s.value("lastDir") == "") {
            globalSettings->s.setValue("lastDir",
                                       QApplication::applicationDirPath());
        }
        // minimum cache size
        if(globalSettings->s.value("cacheSize").toInt() < 32) {
            globalSettings->s.setValue("cacheSize","32");
        }

        if(!ok) {
            qDebug() << "Settings: error reading thumbnail size (int conversion failed).";
            qDebug() << "Settings: setting default size.";
            ok = true;
            globalSettings->s.setValue("thumbnailSize", "120");
        }
    }
}

QString Settings::tempDir() {
    return tempDirectory->path()+"/";
}

QString Settings::ffmpegExecutable() {
    return globalSettings->s.value("ffmpegExe", "").toString();
    //QString tmp = "C:/Users/Eugene/Desktop/ffmpeg/bin/ffmpeg.exe";
    //return tmp;
}

void Settings::setFfmpegExecutable(QString path) {
    globalSettings->s.setValue("ffmpegExe", path);
}

//returns list of regexps
QStringList Settings::supportedFormats() {
    QStringList filters;
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    if(this->playVideos()) {
        supportedFormats << "webm";
    }
    for(int i = 0; i < supportedFormats.count(); i++) {
        filters << "*."+QString(supportedFormats.at(i));
    }
    return filters;
}

// (for open/save dialogs)
// example:  "Images (*.jpg, *.png)"
QString Settings::supportedFormatsString() {
    QString filters;
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    if(this->playVideos()) {
        supportedFormats << "webm";
    }
    filters.append("Images (");
    for(int i = 0; i < supportedFormats.count(); i++) {
        filters.append("*."+QString(supportedFormats.at(i))+" ");
    }
    filters.append(")");
    return filters;
}

bool Settings::playVideos() {
    bool mode = globalSettings->s.value("playVideos", true).toBool();
    return mode;
}

void Settings::setPlayVideos(bool mode) {
    globalSettings->s.setValue("playVideos", mode);
}

bool Settings::playVideoSounds() {
    bool mode = globalSettings->s.value("playVideoSounds", false).toBool();
    return mode;
}

void Settings::setPlayVideoSounds(bool mode) {
    globalSettings->s.setValue("playVideoSounds", mode);
}

/* 0: By name (default)
 * 1: By name reversed
 * 2: By date
 * 3: By date reversed
*/
int Settings::sortingMode() {
    bool ok = true;
    int mode = globalSettings->s.value("sortingMode", "0").toInt(&ok);
    if(!ok) {
        mode = 0;
    }
    return mode;
}

bool Settings::setSortingMode(int mode) {
    if(mode < 0 || mode > 3) {
        qDebug() << "Invalid sorting mode (" << mode << "), resetting to default.";
        mode = 0;
    }
    globalSettings->s.setValue("sortingMode", mode);
}

bool Settings::useFastScale() {
    bool mode = globalSettings->s.value("useFastScale", "false").toBool();
    return mode;
}

void Settings::setUseFastScale(bool mode) {
    globalSettings->s.setValue("useFastScale", mode);
}

QString Settings::lastDirectory() {
    QString dir = globalSettings->s.value("lastDir", "").toString();
    return dir;
}

void Settings::setLastDirectory(QString path) {
    globalSettings->s.setValue("lastDir", path);
}

unsigned int Settings::lastFilePosition() {
    bool ok = true;
    unsigned int pos = globalSettings->s.value("lastFilePosition", "0").toInt(&ok);
    if(!ok) {
        qDebug() << "Settings: Invalid lastFilePosition. Resetting to 0.";
        pos = 0;
    }
    return pos;
}

void Settings::setLastFilePosition(unsigned int pos) {
    globalSettings->s.setValue("lastFilePosition", pos);
}

unsigned int Settings::thumbnailSize() {
    bool ok = true;
    unsigned int size = globalSettings->s.value("thumbnailSize", thumbnailSizeDefault).toInt(&ok);
    if(!ok) {
        size = thumbnailSizeDefault;
    }
    return size;
}

void Settings::setThumbnailSize(unsigned int size) {
    globalSettings->s.setValue("thumbnailSize", size);
}

bool Settings::usePreloader() {
    bool mode = globalSettings->s.value("usePreloader", true).toBool();
    return mode;
}

bool Settings::setUsePreloader(bool mode) {
    globalSettings->s.setValue("usePreloader", mode);
}

QColor Settings::backgroundColor() {
    QColor color = globalSettings->s.value("bgColor", "Qt::Black").value<QColor>();
    return color;
}

void Settings::setBackgroundColor(QColor color) {
    globalSettings->s.setValue("bgColor", color);
}

bool Settings::fullscreenMode() {
    bool mode = true;
    mode = globalSettings->s.value("openInFullscreen", true).toBool();
    return mode;
}

void Settings::setFullscreenMode(bool mode) {
    globalSettings->s.setValue("openInFullscreen", mode);
}

bool Settings::menuBarHidden() {
    bool mode = globalSettings->s.value("hideMenuBar", true).toBool();
    return mode;
}

void Settings::setMenuBarHidden(bool mode) {
    globalSettings->s.setValue("hideMenuBar", mode);
}

/* 0: all
 * 1: fit width
 * 2: orginal size
 */
int Settings::imageFitMode() {
    int mode = globalSettings->s.value("defaultFitMode", 0).toInt();
    if(mode < 0 || mode > 2 ) {
        qDebug() << "Settings: Invalid fit mode ( " + QString::number(mode) + " ). Resetting to default.";
        mode = 0;
    }
    return mode;
}

void Settings::setImageFitMode(int mode) {
    if( mode < 0 || mode > 2 ) {
        qDebug() << "Settings: Invalid fit mode ( " + QString::number(mode) + " ). Resetting to default.";
        mode = 0;
    }
    globalSettings->s.setValue("defaultFitMode", mode);
}

QByteArray Settings::windowGeometry() {
    QByteArray geometry = globalSettings->s.value("windowGeometry").toByteArray();
    return geometry;
}

void Settings::setWindowGeometry(QByteArray geometry) {
    globalSettings->s.setValue("windowGeometry", geometry);
}

bool Settings::reduceRamUsage() {
    bool mode = globalSettings->s.value("reduceRamUsage", false).toBool();
    return mode;
}

void Settings::setReduceRamUsage(bool mode) {
    globalSettings->s.setValue("reduceRamUsage", mode);
}

void Settings::sendChangeNotification() {
    emit settingsChanged();
}

