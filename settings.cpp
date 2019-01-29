#include "settings.h"

Settings *settings = nullptr;

Settings::Settings(QObject *parent) : QObject(parent) {
#ifdef IS_PORTABLE
    cacheDirectory = new QDir(QApplication::applicationDirPath() + "/cache");
    cacheDirectory->mkpath(cacheDirectory->absolutePath());
    thumbnailDirectory = new QDir(QApplication::applicationDirPath() + "/thumbnails");
    thumbnailDirectory->mkpath(thumbnailDirectory->absolutePath());
    s = new QSettings(QApplication::applicationDirPath() + "/qimgv.ini", QSettings::IniFormat);
    state = new QSettings(QApplication::applicationDirPath() + "/savedState.ini", QSettings::IniFormat);
#else
    cacheDirectory = new QDir(QDir::homePath() + "/.cache/qimgv");
    cacheDirectory->mkpath(cacheDirectory->absolutePath());
    thumbnailDirectory = new QDir(QDir::homePath() + "/.cache/qimgv/thumbnails");
    thumbnailDirectory->mkpath(thumbnailDirectory->absolutePath());
    QSettings::setDefaultFormat(QSettings::NativeFormat);
    s = new QSettings();
    state = new QSettings(QCoreApplication::organizationName(), "savedState");
#endif
}

Settings::~Settings() {
    delete thumbnailDirectory;
    delete cacheDirectory;
    delete s;
    delete state;
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
        if(settings->state->value("lastDir") == "") {
            settings->state->setValue("lastDir", QDir::homePath());
        }
        if(!ok) {
            qDebug() << "Settings: error reading thumbnail size (int conversion failed).";
            qDebug() << "Settings: setting default size.";
            ok = true;
            settings->s->setValue("mainPanelSize", 160);
        }
    }
}
//------------------------------------------------------------------------------
void Settings::sync() {
    settings->s->sync();
    settings->state->sync();
}
//------------------------------------------------------------------------------
QString Settings::cacheDir() {
    return thumbnailDirectory->path() + "/";
}
//------------------------------------------------------------------------------
QString Settings::tempDir() {
    return cacheDirectory->path() + "/";
}
//------------------------------------------------------------------------------
QString Settings::mpvBinary() {
    QString mpvPath = settings->s->value("mpvBinary", "").toString();
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
        settings->s->setValue("mpvBinary", path);
    }
}
//------------------------------------------------------------------------------
// returns list of regexps
QStringList Settings::supportedFormats() {
    QStringList filters;
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    if(this->playWebm()) {
        supportedFormats << "webm";
    }
    if(this->playMp4()) {
        supportedFormats << "mp4";
    }
    for(int i = 0; i < supportedFormats.count(); i++) {
        filters << "*." + QString(supportedFormats.at(i));
    }
    return filters;
}
//------------------------------------------------------------------------------
// (for open/save dialogs)
// example:  "Images (*.jpg, *.png)"
QString Settings::supportedFormatsString() {
    QString filters;
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    if(this->playWebm()) {
        supportedFormats << "webm";
    }
    if(this->playMp4()) {
        supportedFormats << "mp4";
    }
    filters.append("Images (");
    for(int i = 0; i < supportedFormats.count(); i++) {
        filters.append("*." + QString(supportedFormats.at(i)) + " ");
    }
    filters.append(")");
    return filters;
}
//------------------------------------------------------------------------------
// returns list of mime types
QStringList Settings::supportedMimeTypes() {
    QStringList filters;
    QList<QByteArray> mimeTypes = QImageReader::supportedMimeTypes();
    if(this->playWebm()) {
        mimeTypes << "video/webm";
    }
    if(this->playMp4()) {
        mimeTypes << "video/mp4";
    }
    for(int i = 0; i < mimeTypes.count(); i++) {
        filters << QString(mimeTypes.at(i));
    }
    //qDebug() << filters;
    return filters;
}
//------------------------------------------------------------------------------
bool Settings::playWebm() {
#ifdef USE_MPV
    return settings->s->value("playWebm", true).toBool();
#else
    return false;
#endif
}

void Settings::setPlayWebm(bool mode) {
    settings->s->setValue("playWebm", mode);
}
//------------------------------------------------------------------------------
bool Settings::playMp4() {
#ifdef USE_MPV
    return settings->s->value("playMp4", false).toBool();
#else
    return false;
#endif
}

void Settings::setPlayMp4(bool mode) {
    settings->s->setValue("playMp4", mode);
}
//------------------------------------------------------------------------------
QVersionNumber Settings::lastVersion() {
    int vmajor = settings->s->value("lastVerMajor", 0).toInt();
    int vminor = settings->s->value("lastVerMinor", 0).toInt();
    int vmicro = settings->s->value("lastVerMicro", 0).toInt();
    return QVersionNumber(vmajor, vminor, vmicro);
}

void Settings::setLastVersion(QVersionNumber &ver) {
    settings->s->setValue("lastVerMajor", ver.majorVersion());
    settings->s->setValue("lastVerMinor", ver.minorVersion());
    settings->s->setValue("lastVerMicro", ver.microVersion());
}
//------------------------------------------------------------------------------
void Settings::setShowChangelogs(bool mode) {
    settings->s->setValue("showChangelogs", mode);
}

bool Settings::showChangelogs() {
    return settings->s->value("showChangelogs", true).toBool();
}
//------------------------------------------------------------------------------
qreal Settings::backgroundOpacity() {
    bool ok = false;
    qreal value = settings->s->value("backgroundOpacity", 1.0).toReal(&ok);
    if(!ok)
        return 0.0;
    if(value > 1.0)
        return 1.0;
    if(value < 0.0)
        return 0.0;
    return value;
}

void Settings::setBackgroundOpacity(qreal value) {
    if(value > 1.0)
        value = 1.0;
    else if(value < 0.0)
        value = 0.0;
    settings->s->setValue("backgroundOpacity", value);
}
//------------------------------------------------------------------------------
bool Settings::blurBackground() {
#ifndef USE_KDE_BLUR
    return false;
#endif
    return settings->s->value("blurBackground", true).toBool();
}

void Settings::setBlurBackground(bool mode) {
    settings->s->setValue("blurBackground", mode);
}
//------------------------------------------------------------------------------
void Settings::setSortingMode(SortingMode mode) {
    if(mode >= 6)
        mode = SortingMode::NAME_ASC;
    settings->s->setValue("sortingMode", mode);
}

SortingMode Settings::sortingMode() {
    int mode = settings->s->value("sortingMode", 0).toInt();
    if(mode < 0 || mode >= 6)
        mode = 0;
    return static_cast<SortingMode>(mode);
}
//------------------------------------------------------------------------------
bool Settings::playVideoSounds() {
    return settings->s->value("playVideoSounds", false).toBool();
}

void Settings::setPlayVideoSounds(bool mode) {
    settings->s->setValue("playVideoSounds", mode);
}
//------------------------------------------------------------------------------
bool Settings::useFastScale() {
    return settings->s->value("useFastScale", "true").toBool();
}

void Settings::setUseFastScale(bool mode) {
    settings->s->setValue("useFastScale", mode);
}
//------------------------------------------------------------------------------
QString Settings::lastDirectory() {
    return settings->state->value("lastDir", QDir::homePath()).toString();
}

void Settings::setLastDirectory(QString path) {
    settings->state->setValue("lastDir", path);
}
//------------------------------------------------------------------------------
unsigned int Settings::lastFilePosition() {
    bool ok = true;
    unsigned int pos = settings->state->value("lastFilePosition", "0").toUInt(&ok);
    if(!ok) {
        qDebug() << "Settings: Invalid lastFilePosition. Resetting to 0.";
        pos = 0;
    }
    return pos;
}

void Settings::setLastFilePosition(unsigned int pos) {
    settings->state->setValue("lastFilePosition", pos);
}
//------------------------------------------------------------------------------
bool Settings::showThumbnailLabels() {
    return settings->s->value("showThumbnailLabels", false).toBool();
}

void Settings::setShowThumbnailLabels(bool mode) {
    settings->s->setValue("showThumbnailLabels", mode);
}
//------------------------------------------------------------------------------
unsigned int Settings::mainPanelSize() {
    bool ok = true;
    unsigned int size = settings->s->value("mainPanelSize", mainPanelSizeDefault).toUInt(&ok);
    if(!ok) {
        size = mainPanelSizeDefault;
    }
    return size;
}

void Settings::setMainPanelSize(unsigned int size) {
    settings->s->setValue("mainPanelSize", size);
}
//------------------------------------------------------------------------------
bool Settings::usePreloader() {
    return settings->s->value("usePreloader", true).toBool();
}

void Settings::setUsePreloader(bool mode) {
    settings->s->setValue("usePreloader", mode);
}
//------------------------------------------------------------------------------
QColor Settings::backgroundColor() {
    return settings->s->value("bgColor", QColor(27, 27, 28)).value<QColor>();
}

void Settings::setBackgroundColor(QColor color) {
    settings->s->setValue("bgColor", color);
}
//------------------------------------------------------------------------------
QColor Settings::backgroundColorFullscreen() {
    return settings->s->value("bgColorFullscreen", QColor(27, 27, 28)).value<QColor>();
}

void Settings::setBackgroundColorFullscreen(QColor color) {
    settings->s->setValue("bgColorFullscreen", color);
}
//------------------------------------------------------------------------------
QColor Settings::accentColor() {
    return settings->s->value("accentColor", QColor(17, 121, 100)).value<QColor>();
}

void Settings::setAccentColor(QColor color) {
    settings->s->setValue("accentColor", color);
}
//------------------------------------------------------------------------------
bool Settings::fullscreenMode() {
    return settings->s->value("openInFullscreen", true).toBool();
}

void Settings::setFullscreenMode(bool mode) {
    settings->s->setValue("openInFullscreen", mode);
}
//------------------------------------------------------------------------------
bool Settings::panelEnabled() {
    return settings->s->value("panelEnabled", true).toBool();
}

void Settings::setPanelEnabled(bool mode) {
    settings->s->setValue("panelEnabled", mode);
}
//------------------------------------------------------------------------------
bool Settings::panelFullscreenOnly() {
    return settings->s->value("panelFullscreenOnly", false).toBool();
}

void Settings::setPanelFullscreenOnly(bool mode) {
    settings->s->setValue("panelFullscreenOnly", mode);
}
//------------------------------------------------------------------------------
int Settings::lastDisplay() {
    return settings->state->value("lastDisplay", 0).toInt();
}

void Settings::setLastDisplay(int display) {
    settings->state->setValue("lastDisplay", display);
}
//------------------------------------------------------------------------------
PanelHPosition Settings::panelPosition() {
    QString posString = settings->s->value("panelPosition", "top").toString();
    if(posString == "top") {
        return PanelHPosition::PANEL_TOP;
    } else {
        return PanelHPosition::PANEL_BOTTOM;
    }
}

void Settings::setPanelPosition(PanelHPosition pos) {
    QString posString;
    switch(pos) {
        case PANEL_TOP:
            posString = "top";
            break;
        case PANEL_BOTTOM:
            posString = "bottom";
            break;
    }
    settings->s->setValue("panelPosition", posString);
}
//------------------------------------------------------------------------------
/*
 * 0: fit window
 * 1: fit width
 * 2: orginal size
 */
ImageFitMode Settings::imageFitMode() {
    int mode = settings->s->value("defaultFitMode", 0).toInt();
    if(mode < 0 || mode > 2) {
        qDebug() << "Settings: Invalid fit mode ( " + QString::number(mode) + " ). Resetting to default.";
        mode = 0;
    }
    return static_cast<ImageFitMode>(mode);
}

void Settings::setImageFitMode(ImageFitMode mode) {
    int modeInt = static_cast<ImageFitMode>(mode);
    if(modeInt < 0 || modeInt > 2) {
        qDebug() << "Settings: Invalid fit mode ( " + QString::number(modeInt) + " ). Resetting to default.";
        modeInt = 0;
    }
    settings->s->setValue("defaultFitMode", modeInt);
}
//------------------------------------------------------------------------------
QRect Settings::windowGeometry() {
    QRect savedRect = settings->state->value("windowGeometry").toRect();
    if(savedRect.size().isEmpty())
        savedRect.setRect(100, 100, 900, 600);
    return savedRect;
}

void Settings::setWindowGeometry(QRect geometry) {
    settings->state->setValue("windowGeometry", geometry);
}
//------------------------------------------------------------------------------
bool Settings::infiniteScrolling() {
    return settings->s->value("infiniteScrolling", false).toBool();
}

void Settings::setInfiniteScrolling(bool mode) {
    settings->s->setValue("infiniteScrolling", mode);
}
//------------------------------------------------------------------------------
void Settings::sendChangeNotification() {
    emit settingsChanged();
}
//------------------------------------------------------------------------------
void Settings::readShortcuts(QMap<QString, QString> &shortcuts) {
    settings->s->beginGroup("Controls");
    QStringList in, pair;
    in = settings->s->value("shortcuts").toStringList();
    for(int i = 0; i < in.count(); i++) {
        pair = in[i].split("=");
        if(!pair[0].isEmpty() && !pair[1].isEmpty()) {
            if(pair[1]=="eq")
                pair[1]="=";
            shortcuts.insert(pair[1], pair[0]);
        }
    }
    settings->s->endGroup();
}

void Settings::saveShortcuts(const QMap<QString, QString> &shortcuts) {
    settings->s->beginGroup("Controls");
    QMapIterator<QString, QString> i(shortcuts);
    QStringList out;
    while(i.hasNext()) {
        i.next();
        if(i.key() == "=")
            out << i.value() + "=" + "eq";
        else
            out << i.value() + "=" + i.key();
    }
    settings->s->setValue("shortcuts", out);
    settings->s->endGroup();
}
//------------------------------------------------------------------------------
void Settings::readScripts(QMap<QString, Script> &scripts) {
    scripts.clear();
    settings->s->beginGroup("Scripts");
    int size = settings->s->beginReadArray("script");
    for(int i=0; i < size; i++) {
        settings->s->setArrayIndex(i);
        QString name = settings->s->value("name").toString();
        QVariant value = settings->s->value("value");
        Script scr = value.value<Script>();
        scripts.insert(name, scr);
    }
    settings->s->endArray();
    settings->s->endGroup();
}

void Settings::saveScripts(const QMap<QString, Script> &scripts) {
    settings->s->beginGroup("Scripts");
    settings->s->beginWriteArray("script");
    QMapIterator<QString, Script> i(scripts);
    int counter = 0;
    while(i.hasNext()) {
        i.next();
        settings->s->setArrayIndex(counter);
        settings->s->setValue("name", i.key());
        settings->s->setValue("value", QVariant::fromValue(i.value()));
        counter++;
    }
    settings->s->endArray();
    settings->s->endGroup();
}
//------------------------------------------------------------------------------
bool Settings::mouseWrapping() {
    return settings->s->value("mouseWrapping", false).toBool();
}

void Settings::setMouseWrapping(bool mode) {
    settings->s->setValue("mouseWrapping", mode);
}
//------------------------------------------------------------------------------
bool Settings::squareThumbnails() {
    return settings->s->value("squareThumbnails", true).toBool();
}

void Settings::setSquareThumbnails(bool mode) {
    settings->s->setValue("squareThumbnails", mode);
}
//------------------------------------------------------------------------------
bool Settings::transparencyGrid() {
    return settings->s->value("drawTransparencyGrid", false).toBool();
}

void Settings::setTransparencyGrid(bool mode) {
    settings->s->setValue("drawTransparencyGrid", mode);
}
//------------------------------------------------------------------------------
bool Settings::enableSmoothScroll() {
    return settings->s->value("enableSmoothScroll", true).toBool();
}

void Settings::setEnableSmoothScroll(bool mode) {
    settings->s->setValue("enableSmoothScroll", mode);
}
//------------------------------------------------------------------------------
bool Settings::useThumbnailCache() {
    return settings->s->value("thumbnailCache", true).toBool();
}

void Settings::setUseThumbnailCache(bool mode) {
    settings->s->setValue("thumbnailCache", mode);
}
//------------------------------------------------------------------------------
QStringList Settings::savedPaths() {
    return settings->state->value("savedPaths").toStringList();
}

void Settings::setSavedPaths(QStringList paths) {
    settings->state->setValue("savedPaths", paths);
}
//------------------------------------------------------------------------------
int Settings::thumbnailerThreadCount() {
    int count = settings->s->value("thumbnailerThreads", 2).toInt();
    if(count < 1)
        count = 1;
    return count;
}

void Settings::setThumbnailerThreadCount(int count) {
    settings->s->setValue("thumbnailerThreads", count);
}
//------------------------------------------------------------------------------
bool Settings::smoothUpscaling() {
    return settings->s->value("smoothUpscaling", true).toBool();
}

void Settings::setSmoothUpscaling(bool mode) {
    settings->s->setValue("smoothUpscaling", mode);
}
//------------------------------------------------------------------------------
int Settings::maxZoomedResolution() {
    return settings->s->value("maximumZoomResolution", 75).toInt();
}

void Settings::setMaxZoomedResolution(int value) {
    settings->s->setValue("maximumZoomResolution", value);
}
//------------------------------------------------------------------------------
int Settings::maximumZoom() {
    return settings->s->value("maximumZoom", 8).toInt();
}

void Settings::setMaximumZoom(int value) {
    settings->s->setValue("maximumZoom", value);
}
//------------------------------------------------------------------------------
int Settings::folderViewIconSize() {
    return settings->s->value("folderViewIconSize", 120).toInt();
}

void Settings::setFolderViewIconSize(int value) {
    settings->s->setValue("folderViewIconSize", value);
}
//------------------------------------------------------------------------------
bool Settings::expandImage() {
    return settings->s->value("expandImage", false).toBool();
}

void Settings::setExpandImage(bool mode) {
    settings->s->setValue("expandImage", mode);
}
//------------------------------------------------------------------------------
/* 0: nearest
 * 1: bilinear
 */
int Settings::scalingFilter() {
    int mode = settings->s->value("scalingFilter", 1).toInt();
    if(mode < 0 || mode > 1)
        mode = 1;
    return mode;
}

void Settings::setScalingFilter(int mode) {
    settings->s->setValue("scalingFilter", mode);
}
//------------------------------------------------------------------------------
bool Settings::smoothAnimatedImages() {
    return settings->s->value("smoothAnimatedImages", true).toBool();
}

void Settings::setSmoothAnimatedImages(bool mode) {
    settings->s->setValue("smoothAnimatedImages", mode);
}
//------------------------------------------------------------------------------
bool Settings::showInfoOverlay() {
    return settings->s->value("showInfoOverlay", true).toBool();
}

void Settings::setShowInfoOverlay(bool mode) {
    settings->s->setValue("showInfoOverlay", mode);
}
//------------------------------------------------------------------------------
bool Settings::firstRun() {
    return settings->s->value("firstRun", true).toBool();
}

void Settings::setFirstRun(bool mode) {
    settings->s->setValue("firstRun", mode);
}
