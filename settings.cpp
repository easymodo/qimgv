#include "settings.h"

Settings *globalSettings=NULL;

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    tempDirectory = new QTemporaryDir(QDir::tempPath()+"/qimgv_tmpXXXXXX");
    tempDirectory->setAutoRemove(true);
}

Settings::~Settings() {
    delete tempDirectory;
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
            globalSettings->s.setValue("thumbnailSize", 135);
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
    QList<QByteArray> supportedFormats = QImageReader::supportedMimeTypes();
    if(this->playVideos()) {
        supportedFormats << "video/webm";
    }
    for(int i = 0; i < supportedFormats.count(); i++) {
        filters << QString(supportedFormats.at(i));
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
    return globalSettings->s.value("playVideos", true).toBool();
}

void Settings::setPlayVideos(bool mode) {
    globalSettings->s.setValue("playVideos", mode);
}

bool Settings::playVideoSounds() {
    return globalSettings->s.value("playVideoSounds", false).toBool();
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

void Settings::setSortingMode(int mode) {
    if(mode < 0 || mode > 3) {
        qDebug() << "Invalid sorting mode (" << mode << "), resetting to default.";
        mode = 0;
    }
    globalSettings->s.setValue("sortingMode", mode);
}

bool Settings::useFastScale() {
    return globalSettings->s.value("useFastScale", "false").toBool();
}

void Settings::setUseFastScale(bool mode) {
    globalSettings->s.setValue("useFastScale", mode);
}

QString Settings::lastDirectory() {
    return globalSettings->s.value("lastDir", "").toString();
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
    return globalSettings->s.value("usePreloader", true).toBool();
}

void Settings::setUsePreloader(bool mode) {
    globalSettings->s.setValue("usePreloader", mode);
}

QColor Settings::backgroundColor() {
    return globalSettings->s.value("bgColor", QColor(14,14,14)).value<QColor>();
}

void Settings::setBackgroundColor(QColor color) {
    globalSettings->s.setValue("bgColor", color);
}

QColor Settings::accentColor() {
    return globalSettings->s.value("accentColor", QColor(0,182,91)).value<QColor>();
}

void Settings::setAccentColor(QColor color) {
    globalSettings->s.setValue("accentColor", color);
}

bool Settings::fullscreenMode() {
    return globalSettings->s.value("openInFullscreen", true).toBool();
}

void Settings::setFullscreenMode(bool mode) {
    globalSettings->s.setValue("openInFullscreen", mode);
}

bool Settings::menuBarHidden() {
    return globalSettings->s.value("hideMenuBar", true).toBool();
}

void Settings::setMenuBarHidden(bool mode) {
    globalSettings->s.setValue("hideMenuBar", mode);
}

bool Settings::showThumbnailLabels() {
    return globalSettings->s.value("showThumbnailLabels", true).toBool();
}

void Settings::setShowThumbnailLabels(bool mode) {
    globalSettings->s.setValue("showThumbnailLabels", mode);
}

PanelPosition Settings::panelPosition() {
    QString posString = globalSettings->s.value("panelPosition", "bottom").toString();
    if(posString == "top") {
        return PanelPosition::TOP;
    } else if(posString == "left") {
        return PanelPosition::LEFT;
    } else if(posString == "right") {
        return PanelPosition::RIGHT;
    } else {
        return PanelPosition::BOTTOM;
    }
}

void Settings::setPanelPosition(PanelPosition pos) {
    QString posString;
    switch(pos) {
        case LEFT: posString = "left"; break;
        case RIGHT: posString = "right"; break;
        case TOP: posString = "top"; break;
        case BOTTOM: posString = "bottom"; break;
    }
    globalSettings->s.setValue("panelPosition", posString);
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
    return globalSettings->s.value("windowGeometry").toByteArray();
}

void Settings::setWindowGeometry(QByteArray geometry) {
    globalSettings->s.setValue("windowGeometry", geometry);
}

bool Settings::reduceRamUsage() {
    return globalSettings->s.value("reduceRamUsage", false).toBool();
}

void Settings::setReduceRamUsage(bool mode) {
    globalSettings->s.setValue("reduceRamUsage", mode);
}

bool Settings::infiniteScrolling() {
    return globalSettings->s.value("infiniteScrolling", false).toBool();
}

void Settings::setInfiniteScrolling(bool mode) {
    globalSettings->s.setValue("infiniteScrolling", mode);
}

void Settings::sendChangeNotification() {
    emit settingsChanged();
}

