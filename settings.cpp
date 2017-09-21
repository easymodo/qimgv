#include "settings.h"

Settings *settings = NULL;

Settings::Settings(QObject *parent) : QObject(parent) {
    tempDirectory = new QDir(QDir::homePath() + "/.cache/qimgv");
    tempDirectory->mkpath(tempDirectory->absolutePath());

    cacheDirectory = new QDir(QDir::homePath() + "/.cache/qimgv/thumbnails");
    cacheDirectory->mkpath(cacheDirectory->absolutePath());
}

Settings::~Settings() {
    delete cacheDirectory;
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
                                 QDir::homePath());
        }
        // minimum cache size
        if(settings->s.value("cacheSize").toInt() < 32) {
            settings->s.setValue("cacheSize", "32");
        }

        if(!ok) {
            qDebug() << "Settings: error reading thumbnail size (int conversion failed).";
            qDebug() << "Settings: setting default size.";
            ok = true;
            settings->s.setValue("mainPanelSize", 160);
        }
    }
}

QString Settings::cacheDir() {
    return cacheDirectory->path() + "/";
}

QString Settings::tempDir() {
    return tempDirectory->path() + "/";
}

QString Settings::mpvBinary() {
    QString mpvPath = settings->s.value("mpvBinary", "").toString();
    if(!QFile::exists(mpvPath)) {
        #ifdef _WIN32
        mpvPath = QCoreApplication::applicationDirPath() + "/mpv.exe";
        #elif defined __linux__
        mpvPath = "/usr/bin/mpv";
        #endif
        if(!QFile::exists(mpvPath)) {
            mpvPath = "";
        }

    }
    return mpvPath;
}

void Settings::setMpvBinary(QString path) {
    if(QFile::exists(path)) {
        settings->s.setValue("mpvBinary", path);
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
    //qDebug() << filters;
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

/*
 * 0: By name (default)
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
    return settings->s.value("lastDir", QDir::homePath()).toString();
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

unsigned int Settings::mainPanelSize() {
    bool ok = true;
    unsigned int size = settings->s.value("mainPanelSize", mainPanelSizeDefault).toInt(&ok);
    if(!ok) {
        size = mainPanelSizeDefault;
    }
    return size;
}

void Settings::setMainPanelSize(unsigned int size) {
    settings->s.setValue("mainPanelSize", size);
}

bool Settings::usePreloader() {
    return settings->s.value("usePreloader", true).toBool();
}

void Settings::setUsePreloader(bool mode) {
    settings->s.setValue("usePreloader", mode);
}

QColor Settings::backgroundColor() {
    return settings->s.value("bgColor", QColor(27, 27, 27)).value<QColor>();
}

void Settings::setBackgroundColor(QColor color) {
    settings->s.setValue("bgColor", color);
}

QColor Settings::accentColor() {
    return settings->s.value("accentColor", QColor(104, 159, 56)).value<QColor>();
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

bool Settings::showThumbnailLabels() {
    return settings->s.value("showThumbnailLabels", true).toBool();
}

void Settings::setShowThumbnailLabels(bool mode) {
    settings->s.setValue("showThumbnailLabels", mode);
}

bool Settings::mainPanelEnabled() {
    return settings->s.value("panelEnabled", true).toBool();
}

void Settings::setPanelEnabled(bool mode) {
    settings->s.setValue("panelEnabled", mode);
}

bool Settings::sidePanelEnabled() {
    return settings->s.value("sidePanelEnabled", false).toBool();
}

void Settings::setSidePanelEnabled(bool mode) {
    settings->s.setValue("sidePanelEnabled", mode);
}

int Settings::lastDisplay() {
    return settings->s.value("lastDisplay", 0).toInt();
}

void Settings::setLastDisplay(int display) {
    settings->s.setValue("lastDisplay", display);
}

PanelHPosition Settings::panelPosition() {
    QString posString = settings->s.value("panelPosition", "top").toString();
    if(posString == "top") {
        return PanelHPosition::PANEL_TOP;
    } else {
        return PanelHPosition::PANEL_BOTTOM;
    }
}

void Settings::setPanelPosition(PanelHPosition pos) {
    QString posString;
    switch(pos) {
            break;
        case PANEL_TOP:
            posString = "top";
            break;
        case PANEL_BOTTOM:
            posString = "bottom";
            break;
    }
    settings->s.setValue("panelPosition", posString);
}

PanelVPosition Settings::sidePanelPosition() {
    QString posString = settings->s.value("sidePanelPosition", "right").toString();
    if(posString == "right") {
        return PanelVPosition::PANEL_RIGHT;
    } else {
        return PanelVPosition::PANEL_LEFT;
    }
}

void Settings::setSidePanelPosition(PanelVPosition pos) {
    QString posString;
    switch(pos) {
            break;
        case PANEL_RIGHT:
            posString = "right";
            break;
        case PANEL_LEFT:
            posString = "left";
            break;
    }
    settings->s.setValue("sidePanelPosition", posString);
}

/*
 * 0: all
 * 1: fit width
 * 2: orginal size
 */
ImageFitMode Settings::imageFitMode() {
    int mode = settings->s.value("defaultFitMode", 0).toInt();
    if(mode < 0 || mode > 2) {
        qDebug() << "Settings: Invalid fit mode ( " + QString::number(mode) + " ). Resetting to default.";
        mode = 0;
    }
    return (ImageFitMode)mode;
}

void Settings::setImageFitMode(ImageFitMode mode) {
    int modeInteger = (int)mode;
    if(modeInteger < 0 || modeInteger > 2) {
        qDebug() << "Settings: Invalid fit mode ( " + QString::number(modeInteger) + " ). Resetting to default.";
        modeInteger = 0;
    }
    settings->s.setValue("defaultFitMode", modeInteger);
}

QRect Settings::windowGeometry() {
    QRect savedRect = settings->s.value("windowGeometry").toRect();
    if(savedRect.size().isEmpty())
        savedRect.setRect(100, 100, 900, 600);
    return savedRect;
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

bool Settings::mouseWrapping() {
    return settings->s.value("mouseWrapping", false).toBool();
}

void Settings::setMouseWrapping(bool mode) {
    settings->s.setValue("mouseWrapping", mode);
}

bool Settings::squareThumbnails() {
    return settings->s.value("squareThumbnails", true).toBool();
}

void Settings::setSquareThumbnails(bool mode) {
    settings->s.setValue("squareThumbnails", mode);
}

bool Settings::transparencyGrid() {
    return settings->s.value("drawTransparencyGrid", true).toBool();
}

void Settings::setTransparencyGrid(bool mode) {
    settings->s.setValue("drawTransparencyGrid", mode);
}

bool Settings::forceSmoothScroll() {
    return settings->s.value("forceSmoothScroll", false).toBool();
}

void Settings::setForceSmoothScroll(bool mode) {
    settings->s.setValue("forceSmoothScroll", mode);
}

bool Settings::useThumbnailCache() {
    return settings->s.value("thumbnailCache", true).toBool();
}

void Settings::setUseThumbnailCache(bool mode) {
    settings->s.setValue("thumbnailCache", mode);
}

QStringList Settings::savedPaths() {
    return settings->s.value("savedPaths").toStringList();
}

void Settings::setSavedPaths(QStringList paths) {
    settings->s.setValue("savedPaths", paths);
}

int Settings::thumbnailerThreadCount() {
    int count = settings->s.value("thumbnailerThreads", 2).toInt();
    if(count <= 0)
        count = 2;
    return count;
}

void Settings::setThumbnailerThreadCount(int count) {
    settings->s.setValue("thumbnailerThreads", count);
}

bool Settings::smoothUpscaling() {
    return settings->s.value("smoothUpscaling", true).toBool();
}

void Settings::setSmoothUpscaling(bool mode) {
    settings->s.setValue("smoothUpscaling", mode);
}

int Settings::maxZoomedResolution() {
    return settings->s.value("maximumZoomResolution", 75).toInt();
}

void Settings::setMaxZoomedResolution(int value) {
    settings->s.setValue("maximumZoomResolution", value);
}

int Settings::maximumZoom() {
    return settings->s.value("maximumZoom", 4).toInt();
}

void Settings::setMaximumZoom(int value) {
    settings->s.setValue("maximumZoom", value);
}

bool Settings::expandImage() {
    return settings->s.value("expandImage", false).toBool();
}

void Settings::setExpandImage(bool mode) {
    settings->s.setValue("expandImage", mode);
}

bool Settings::fadeEffect() {
    return settings->s.value("fadeEffect", true).toBool();
}

void Settings::setFadeEffect(bool mode) {
    settings->s.setValue("fadeEffect", mode);
}


