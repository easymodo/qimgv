#include "settings.h"

Settings *settings = NULL;

Settings::Settings(QObject *parent) :
    QObject(parent) {
    tempDirectory = new QDir(QDir::tempPath() + "/qimgv");
    tempDirectory->mkpath(QDir::tempPath() + "/qimgv");
}

Settings::~Settings() {
    delete tempDirectory;
}

Settings *Settings::getInstance() {
    if(!settings) {
        settings = new Settings();
        validate();
    }
    return settings;
}

void Settings::validate() {
    if(settings) {
        bool ok = true;
        if(settings->s.value("lastDir") == "") {
            settings->s.setValue("lastDir",
                                 QApplication::applicationDirPath());
        }
        // minimum cache size
        if(settings->s.value("cacheSize").toInt() < 32) {
            settings->s.setValue("cacheSize", "32");
        }

        if(!ok) {
            qDebug() << "Settings: error reading thumbnail size (int conversion failed).";
            qDebug() << "Settings: setting default size.";
            ok = true;
            settings->s.setValue("thumbnailSize", 160);
        }
    }
}

QString Settings::tempDir() {
    return tempDirectory->path() + "/";
}

QString Settings::ffmpegExecutable() {
    QString ffmpegPath = settings->s.value("ffmpegExe", "").toString();
    if(!QFile::exists(ffmpegPath)) {
        #ifdef _WIN32
        ffmpegPath = QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
        #elif defined __linux__
        ffmpegPath = "/usr/bin/ffmpeg";
        #endif
        if(!QFile::exists(ffmpegPath)) {
            ffmpegPath = "";
        }

    }
    return ffmpegPath;
}

void Settings::setFfmpegExecutable(QString path) {
    if(QFile::exists(path)) {
        settings->s.setValue("ffmpegExe", path);
    }
}

// returns list of regexps
QStringList Settings::supportedFormats() {
    QStringList filters;
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    if(this->playVideos()) {
        supportedFormats << "webm";
    }
    for(int i = 0; i < supportedFormats.count(); i++) {
        filters << "*." + QString(supportedFormats.at(i));
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
        filters.append("*." + QString(supportedFormats.at(i)) + " ");
    }
    filters.append(")");
    return filters;
}

// returns list of mime types
QStringList Settings::supportedMimeTypes() {
    QStringList filters;
    QList<QByteArray> mimeTypes = QImageReader::supportedMimeTypes();
    if(this->playVideos()) {
        mimeTypes << "video/webm";
    }
    for(int i = 0; i < mimeTypes.count(); i++) {
        filters << QString(mimeTypes.at(i));
    }
    qDebug() << filters;
    return filters;
}

bool Settings::playVideos() {
    return settings->s.value("playVideos", true).toBool();
}

void Settings::setPlayVideos(bool mode) {
    settings->s.setValue("playVideos", mode);
}

bool Settings::playVideoSounds() {
    return settings->s.value("playVideoSounds", false).toBool();
}

void Settings::setPlayVideoSounds(bool mode) {
    settings->s.setValue("playVideoSounds", mode);
}

/* 0: By name (default)
 * 1: By name reversed
 * 2: By date
 * 3: By date reversed
*/
int Settings::sortingMode() {
    bool ok = true;
    int mode = settings->s.value("sortingMode", "0").toInt(&ok);
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
    settings->s.setValue("sortingMode", mode);
}

bool Settings::useFastScale() {
    return settings->s.value("useFastScale", "true").toBool();
}

void Settings::setUseFastScale(bool mode) {
    settings->s.setValue("useFastScale", mode);
}

QString Settings::lastDirectory() {
    return settings->s.value("lastDir", "").toString();
}

void Settings::setLastDirectory(QString path) {
    settings->s.setValue("lastDir", path);
}

unsigned int Settings::lastFilePosition() {
    bool ok = true;
    unsigned int pos = settings->s.value("lastFilePosition", "0").toInt(&ok);
    if(!ok) {
        qDebug() << "Settings: Invalid lastFilePosition. Resetting to 0.";
        pos = 0;
    }
    return pos;
}

void Settings::setLastFilePosition(unsigned int pos) {
    settings->s.setValue("lastFilePosition", pos);
}

unsigned int Settings::thumbnailSize() {
    bool ok = true;
    unsigned int size = settings->s.value("thumbnailSize", thumbnailSizeDefault).toInt(&ok);
    if(!ok) {
        size = thumbnailSizeDefault;
    }
    return size;
}

void Settings::setThumbnailSize(unsigned int size) {
    settings->s.setValue("thumbnailSize", size);
}

bool Settings::usePreloader() {
    return settings->s.value("usePreloader", true).toBool();
}

void Settings::setUsePreloader(bool mode) {
    settings->s.setValue("usePreloader", mode);
}

QColor Settings::backgroundColor() {
    return settings->s.value("bgColor", QColor(14, 14, 14)).value<QColor>();
}

void Settings::setBackgroundColor(QColor color) {
    settings->s.setValue("bgColor", color);
}

QColor Settings::accentColor() {
    return settings->s.value("accentColor", QColor(44, 180, 13)).value<QColor>();
}

void Settings::setAccentColor(QColor color) {
    settings->s.setValue("accentColor", color);
}

bool Settings::fullscreenMode() {
    return settings->s.value("openInFullscreen", true).toBool();
}

void Settings::setFullscreenMode(bool mode) {
    settings->s.setValue("openInFullscreen", mode);
}

bool Settings::menuBarHidden() {
    return settings->s.value("hideMenuBar", true).toBool();
}

void Settings::setMenuBarHidden(bool mode) {
    settings->s.setValue("hideMenuBar", mode);
}

// maximized borderless window instead of fullscreen
bool Settings::fullscreenTaskbarShown() {
    return settings->s.value("fullscreenTaskbarShown", false).toBool();
}

void Settings::setFullscreenTaskbarShown(bool mode) {
    settings->s.setValue("fullscreenTaskbarShown", mode);
}

bool Settings::showThumbnailLabels() {
    return settings->s.value("showThumbnailLabels", true).toBool();
}

void Settings::setShowThumbnailLabels(bool mode) {
    settings->s.setValue("showThumbnailLabels", mode);
}

bool Settings::panelEnabled() {
    return settings->s.value("panelEnabled", true).toBool();
}

bool Settings::setPanelEnabled(bool mode) {
    settings->s.setValue("panelEnabled", mode);
}

int Settings::lastDisplay() {
    return settings->s.value("lastDisplay", 0).toInt();
}

int Settings::setLastDisplay(int display) {
    settings->s.setValue("lastDisplay", display);
}

PanelPosition Settings::panelPosition() {
    QString posString = settings->s.value("panelPosition", "top").toString();
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
        case LEFT:
            posString = "left";
            break;
        case RIGHT:
            posString = "right";
            break;
        case TOP:
            posString = "top";
            break;
        case BOTTOM:
            posString = "bottom";
            break;
    }
    settings->s.setValue("panelPosition", posString);
}

/* 0: all
 * 1: fit width
 * 2: orginal size
 */
int Settings::imageFitMode() {
    int mode = settings->s.value("defaultFitMode", 0).toInt();
    if(mode < 0 || mode > 2) {
        qDebug() << "Settings: Invalid fit mode ( " + QString::number(mode) + " ). Resetting to default.";
        mode = 0;
    }
    return mode;
}

void Settings::setImageFitMode(int mode) {
    if(mode < 0 || mode > 2) {
        qDebug() << "Settings: Invalid fit mode ( " + QString::number(mode) + " ). Resetting to default.";
        mode = 0;
    }
    settings->s.setValue("defaultFitMode", mode);
}

QRect Settings::windowGeometry() {
    return settings->s.value("windowGeometry").toRect();
}

void Settings::setWindowGeometry(QRect geometry) {
    settings->s.setValue("windowGeometry", geometry);
}

bool Settings::reduceRamUsage() {
    return settings->s.value("reduceRamUsage", false).toBool();
}

void Settings::setReduceRamUsage(bool mode) {
    settings->s.setValue("reduceRamUsage", mode);
}

bool Settings::infiniteScrolling() {
    return settings->s.value("infiniteScrolling", false).toBool();
}

void Settings::setInfiniteScrolling(bool mode) {
    settings->s.setValue("infiniteScrolling", mode);
}

void Settings::sendChangeNotification() {
    emit settingsChanged();
}

void Settings::readShortcuts() {
    settings->s.beginGroup("Controls");
    QStringList in, pair;
    in = settings->s.value("shortcuts").toStringList();
    for(int i = 0; i < in.count(); i++) {
        pair = in[i].split("=");
        if(!pair[0].isEmpty() && !pair[1].isEmpty()) {
            actionManager->addShortcut(pair[1], pair[0]);
        }
    }
    settings->s.endGroup();
}

void Settings::saveShortcuts() {
    settings->s.beginGroup("Controls");
    const QMap<QString, QString> &shortcuts = actionManager->allShortcuts();
    QMapIterator<QString, QString> i(shortcuts);
    QStringList out;
    while(i.hasNext()) {
        i.next();
        out << i.value() + "=" + i.key();
    }
    settings->s.setValue("shortcuts", out);
    settings->s.endGroup();
}
