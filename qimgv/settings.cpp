#include "settings.h"

Settings *settings = nullptr;

Settings::Settings(QObject *parent) : QObject(parent) {
#ifdef __linux__
    // config files
    QSettings::setDefaultFormat(QSettings::NativeFormat);
    settingsConf = new QSettings();
    stateConf = new QSettings(QCoreApplication::organizationName(), "savedState");
    themeConf = new QSettings(QCoreApplication::organizationName(), "theme");
#else
    mConfDir = new QDir(QApplication::applicationDirPath() + "/conf");
    mConfDir->mkpath(QApplication::applicationDirPath() + "/conf");
    settingsConf = new QSettings(mConfDir->absolutePath() + "/" + qApp->applicationName() + ".ini", QSettings::IniFormat);
    stateConf = new QSettings(mConfDir->absolutePath() + "/savedState.ini", QSettings::IniFormat);
    themeConf = new QSettings(mConfDir->absolutePath() + "/theme.ini", QSettings::IniFormat);
#endif
    loadTheme();
}
//------------------------------------------------------------------------------
Settings::~Settings() {
    saveTheme();
    delete mThumbCacheDir;
    delete mTmpDir;
    delete settingsConf;
    delete stateConf;
    delete themeConf;
}
//------------------------------------------------------------------------------
Settings *Settings::getInstance() {
    if(!settings) {
        settings = new Settings();
        settings->setupCache();
    }
    return settings;
}
//------------------------------------------------------------------------------
void Settings::setupCache() {
#ifdef __linux__
    QString genericCacheLocation = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    if(genericCacheLocation.isEmpty())
        genericCacheLocation = QDir::homePath() + "/.cache";
    genericCacheLocation.append("/" + QApplication::applicationName());
    QString cacheLocation = settings->settingsConf->value("cacheDir", genericCacheLocation).toString();
    mTmpDir = new QDir(cacheLocation);
    mTmpDir->mkpath(mTmpDir->absolutePath());
    QFileInfo dirTest(mTmpDir->absolutePath());
    if(!dirTest.isDir() || !dirTest.isWritable() || !dirTest.exists()) {
        // fallback
        qDebug() << "Error: cache dir is not writable" << mTmpDir->absolutePath();
        qDebug() << "Trying to use" << genericCacheLocation << "instead";
        mTmpDir->setPath(genericCacheLocation);
        mTmpDir->mkpath(mTmpDir->absolutePath());
    }
    mThumbCacheDir = new QDir(mTmpDir->absolutePath() + "/thumbnails");
    mThumbCacheDir->mkpath(mThumbCacheDir->absolutePath());
#else
    mTmpDir = new QDir(QApplication::applicationDirPath() + "/cache");
    mTmpDir->mkpath(mTmpDir->absolutePath());
    mThumbCacheDir = new QDir(QApplication::applicationDirPath() + "/thumbnails");
    mThumbCacheDir->mkpath(mThumbCacheDir->absolutePath());
#endif
}
//------------------------------------------------------------------------------
void Settings::sync() {
    settings->settingsConf->sync();
    settings->stateConf->sync();
}
//------------------------------------------------------------------------------
QString Settings::thumbnailCacheDir() {
    return mThumbCacheDir->path() + "/";
}
//------------------------------------------------------------------------------
QString Settings::tmpDir() {
    return mTmpDir->path() + "/";
}
//------------------------------------------------------------------------------
void Settings::loadTheme() {
    ColorScheme colors;
    themeConf->beginGroup("Colors");
    colors.background            = QColor(themeConf->value("background",            "#1a1a1a").toString());
    colors.background_fullscreen = QColor(themeConf->value("background_fullscreen", "#1a1a1a").toString());
    colors.text                  = QColor(themeConf->value("text",                  "#a4a4a4").toString());
    colors.widget                = QColor(themeConf->value("widget",                "#252525").toString());
    colors.widget_border         = QColor(themeConf->value("widget_border",         "#272727").toString());
    colors.accent                = QColor(themeConf->value("accent",                "#4f6a91").toString());
    colors.folderview            = QColor(themeConf->value("folderview",            "#242424").toString());
    colors.folderview_topbar     = QColor(themeConf->value("folderview_topbar",     "#343434").toString());
    colors.slider_handle         = QColor(themeConf->value("slider_handle",         "#5e5e5e").toString());
    colors.overlay_text          = QColor(themeConf->value("overlay_text",          "#d2d2d2").toString());
    colors.overlay               = QColor(themeConf->value("overlay",               "#1a1a1a").toString());
    themeConf->endGroup();
    setColorScheme(colors);

    QPalette p;
    // choose icons depending on text color
    if(p.text().color().valueF() > 0.5f) {
        mTheme.systemIconTheme = "light";
    } else {
        mTheme.systemIconTheme = "dark";
    }
    QColor system_window = p.window().color();
    if(system_window.valueF() <= 0.45f) {
        mTheme.system_window_tinted.setHsv(system_window.hue(),
                                           system_window.saturation(),
                                           system_window.value() + 16);
    } else {
        mTheme.system_window_tinted.setHsv(system_window.hue(),
                                           system_window.saturation(),
                                           system_window.value() - 16);
    }
}
void Settings::saveTheme() {
    themeConf->beginGroup("Colors");
    themeConf->setValue("background",            mTheme.colors.background.name());
    themeConf->setValue("background_fullscreen", mTheme.colors.background_fullscreen.name());
    themeConf->setValue("text",                  mTheme.colors.text.name());
    themeConf->setValue("widget",                mTheme.colors.widget.name());
    themeConf->setValue("widget_border",         mTheme.colors.widget_border.name());
    themeConf->setValue("accent",                mTheme.colors.accent.name());
    themeConf->setValue("folderview",            mTheme.colors.folderview.name());
    themeConf->setValue("folderview_topbar",     mTheme.colors.folderview_topbar.name());
    themeConf->setValue("slider_handle",         mTheme.colors.slider_handle.name());
    themeConf->setValue("overlay_text",          mTheme.colors.overlay_text.name());
    themeConf->setValue("overlay",               mTheme.colors.overlay.name());
    themeConf->endGroup();
}
//------------------------------------------------------------------------------
const Theme &Settings::theme() {
    return mTheme;
}
//------------------------------------------------------------------------------
const ColorScheme& Settings::colorScheme() {
    return mTheme.colors;
}
//------------------------------------------------------------------------------
void Settings::setColorScheme(ColorScheme &scheme) {
    mTheme.colors = scheme;
    // choose icons depending on text color
    if(mTheme.colors.text.valueF() > 0.5f)
        mTheme.iconTheme = "light";
    else
        mTheme.iconTheme = "dark";
    qDebug() << mTheme.colors.text.valueF() << mTheme.iconTheme;
    createColorVariants();
}
//------------------------------------------------------------------------------
void Settings::createColorVariants() {
    // light variant needs tweaking
    //if(mTheme.colors.widget.valueF() <= 0.45f) {
        // top bar buttons
        mTheme.colors.panel_button_hover.setHsv(mTheme.colors.folderview_topbar.hue(),
                                                mTheme.colors.folderview_topbar.saturation(),
                                                qMin(mTheme.colors.folderview_topbar.value() + 30, 255));
        mTheme.colors.panel_button_pressed.setHsv(mTheme.colors.folderview_topbar.hue(),
                                                  mTheme.colors.folderview_topbar.saturation(),
                                                  qMin(mTheme.colors.folderview_topbar.value() + 20, 255));


        mTheme.colors.folderview_separator.setHsv(mTheme.colors.folderview.hue(),
                                                  mTheme.colors.folderview.saturation(),
                                                  qMin(mTheme.colors.folderview.value() + 22, 255));

        // regular buttons - from widget bg
        mTheme.colors.button.setHsv(mTheme.colors.widget.hue(),
                                    mTheme.colors.widget.saturation(),
                                    qMin(mTheme.colors.widget.value() + 20, 255));
        mTheme.colors.button_hover   = QColor(mTheme.colors.button.lighter(112));
        mTheme.colors.button_pressed = QColor(mTheme.colors.button.darker(112));
        mTheme.colors.button_border  = QColor(mTheme.colors.button.darker(145));

        // accents (saturation tweak)
        mTheme.colors.accent_lc.setHsv(mTheme.colors.accent.hue(),
                                       mTheme.colors.accent.saturation() * 0.9,
                                       mTheme.colors.accent.value() * 0.8f);
        mTheme.colors.accent_lc2.setHsv(mTheme.colors.accent.hue(),
                                        mTheme.colors.accent.saturation() * 0.5,
                                        mTheme.colors.accent.value() * 0.62f);
        mTheme.colors.accent_hc.setHsv(mTheme.colors.accent.hue(),
                                       qMin(static_cast<int>(mTheme.colors.accent.saturation() * 1.1), 255),
                                       qMin(static_cast<int>(mTheme.colors.accent.value() * 1.2), 255));

        // text
        mTheme.colors.text_hc2 = QColor(mTheme.colors.text.lighter(118));
        mTheme.colors.text_hc1 = QColor(mTheme.colors.text.lighter(110));
        mTheme.colors.text_lc1 = QColor(mTheme.colors.text.darker(104));
        mTheme.colors.text_lc2 = QColor(mTheme.colors.text.darker(112));

        // misc
        mTheme.colors.input_field_focus = QColor(mTheme.colors.accent);
        mTheme.colors.slider_hover      = QColor(mTheme.colors.slider_handle.lighter(120));
        mTheme.colors.slider_groove     = mTheme.colors.folderview;
    //} else { // light variant - todo!!

    //}
}
//------------------------------------------------------------------------------
QString Settings::mpvBinary() {
    QString mpvPath = settings->settingsConf->value("mpvBinary", "").toString();
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
        settings->settingsConf->setValue("mpvBinary", path);
    }
}
//------------------------------------------------------------------------------
QList<QByteArray> Settings::supportedFormats() {
    auto formats = QImageReader::supportedImageFormats();
    formats << "jfif";
    if(videoPlayback())
        formats << "webm" << "mp4";
    formats.removeAll("pdf");
    return formats;
}
//------------------------------------------------------------------------------
// (for open/save dialogs)
// example:  "Images (*.jpg, *.png)"
QString Settings::supportedFormatsString() {
    QString filters;
    auto formats = supportedFormats();
    filters.append("Supported files (");
    for(int i = 0; i < formats.count(); i++) {
        filters.append("*." + QString(formats.at(i)) + " ");
    }
    filters.append(")");
    return filters;
}
//------------------------------------------------------------------------------
QString Settings::supportedFormatsRegex() {
    QString filter;
    QList<QByteArray> formats = supportedFormats();
    filter.append(".*\\.(");
    for(int i = 0; i < formats.count(); i++) {
        filter.append(QString(formats.at(i)) + "|");
    }
    filter.chop(1);
    filter.append(")$");
    return filter;
}
//------------------------------------------------------------------------------
// returns list of mime types
QStringList Settings::supportedMimeTypes() {
    QStringList filters;
    QList<QByteArray> mimeTypes = QImageReader::supportedMimeTypes();
    if(videoPlayback())
        mimeTypes << "video/webm" << "video/mp4";
    for(int i = 0; i < mimeTypes.count(); i++) {
        filters << QString(mimeTypes.at(i));
    }
    return filters;
}
//------------------------------------------------------------------------------
bool Settings::videoPlayback() {
#ifdef USE_MPV
    return settings->settingsConf->value("videoPlayback", true).toBool();
#else
    return false;
#endif
}

void Settings::setVideoPlayback(bool mode) {
    settings->settingsConf->setValue("videoPlayback", mode);
}
//------------------------------------------------------------------------------
QVersionNumber Settings::lastVersion() {
    int vmajor = settings->settingsConf->value("lastVerMajor", 0).toInt();
    int vminor = settings->settingsConf->value("lastVerMinor", 0).toInt();
    int vmicro = settings->settingsConf->value("lastVerMicro", 0).toInt();
    return QVersionNumber(vmajor, vminor, vmicro);
}

void Settings::setLastVersion(QVersionNumber &ver) {
    settings->settingsConf->setValue("lastVerMajor", ver.majorVersion());
    settings->settingsConf->setValue("lastVerMinor", ver.minorVersion());
    settings->settingsConf->setValue("lastVerMicro", ver.microVersion());
}
//------------------------------------------------------------------------------
void Settings::setShowChangelogs(bool mode) {
    settings->settingsConf->setValue("showChangelogs", mode);
}

bool Settings::showChangelogs() {
    return settings->settingsConf->value("showChangelogs", true).toBool();
}
//------------------------------------------------------------------------------
qreal Settings::backgroundOpacity() {
    bool ok = false;
    qreal value = settings->settingsConf->value("backgroundOpacity", 1.0).toReal(&ok);
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
    settings->settingsConf->setValue("backgroundOpacity", value);
}
//------------------------------------------------------------------------------
bool Settings::blurBackground() {
#ifndef USE_KDE_BLUR
    return false;
#endif
    return settings->settingsConf->value("blurBackground", true).toBool();
}

void Settings::setBlurBackground(bool mode) {
    settings->settingsConf->setValue("blurBackground", mode);
}
//------------------------------------------------------------------------------
void Settings::setSortingMode(SortingMode mode) {
    if(mode >= 6)
        mode = SortingMode::SORT_NAME;
    settings->settingsConf->setValue("sortingMode", mode);
}

SortingMode Settings::sortingMode() {
    int mode = settings->settingsConf->value("sortingMode", 0).toInt();
    if(mode < 0 || mode >= 6)
        mode = 0;
    return static_cast<SortingMode>(mode);
}
//------------------------------------------------------------------------------
bool Settings::playVideoSounds() {
    return settings->settingsConf->value("playVideoSounds", false).toBool();
}

void Settings::setPlayVideoSounds(bool mode) {
    settings->settingsConf->setValue("playVideoSounds", mode);
}
//------------------------------------------------------------------------------
void Settings::setVolume(int vol) {
    settings->stateConf->setValue("volume", vol);
}

int Settings::volume() {
    return settings->stateConf->value("volume", 100).toInt();
}
//------------------------------------------------------------------------------
bool Settings::showThumbnailLabels() {
    return settings->settingsConf->value("showThumbnailLabels", false).toBool();
}

void Settings::setShowThumbnailLabels(bool mode) {
    settings->settingsConf->setValue("showThumbnailLabels", mode);
}
//------------------------------------------------------------------------------
int Settings::mainPanelSize() {
    bool ok = true;
    int size = settings->settingsConf->value("mainPanelSize", mainPanelSizeDefault).toInt(&ok);
    if(!ok)
        size = mainPanelSizeDefault;
    size = qBound(160, size, 350);
    return size;
}

void Settings::setMainPanelSize(int size) {
    settings->settingsConf->setValue("mainPanelSize", size);
}
//------------------------------------------------------------------------------
bool Settings::usePreloader() {
    return settings->settingsConf->value("usePreloader", true).toBool();
}

void Settings::setUsePreloader(bool mode) {
    settings->settingsConf->setValue("usePreloader", mode);
}
//------------------------------------------------------------------------------
bool Settings::keepFitMode() {
    return settings->settingsConf->value("keepFitMode", false).toBool();
}

void Settings::setKeepFitMode(bool mode) {
    settings->settingsConf->setValue("keepFitMode", mode);
}
//------------------------------------------------------------------------------
bool Settings::fullscreenMode() {
    return settings->settingsConf->value("openInFullscreen", false).toBool();
}

void Settings::setFullscreenMode(bool mode) {
    settings->settingsConf->setValue("openInFullscreen", mode);
}
//------------------------------------------------------------------------------
bool Settings::maximizedWindow() {
    return settings->stateConf->value("maximizedWindow", false).toBool();
}

void Settings::setMaximizedWindow(bool mode) {
    settings->stateConf->setValue("maximizedWindow", mode);
}
//------------------------------------------------------------------------------
bool Settings::panelEnabled() {
    return settings->settingsConf->value("panelEnabled", true).toBool();
}

void Settings::setPanelEnabled(bool mode) {
    settings->settingsConf->setValue("panelEnabled", mode);
}
//------------------------------------------------------------------------------
bool Settings::panelFullscreenOnly() {
    return settings->settingsConf->value("panelFullscreenOnly", true).toBool();
}

void Settings::setPanelFullscreenOnly(bool mode) {
    settings->settingsConf->setValue("panelFullscreenOnly", mode);
}
//------------------------------------------------------------------------------
int Settings::lastDisplay() {
    return settings->stateConf->value("lastDisplay", 0).toInt();
}

void Settings::setLastDisplay(int display) {
    settings->stateConf->setValue("lastDisplay", display);
}
//------------------------------------------------------------------------------
PanelHPosition Settings::panelPosition() {
    QString posString = settings->settingsConf->value("panelPosition", "top").toString();
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
    settings->settingsConf->setValue("panelPosition", posString);
}
//------------------------------------------------------------------------------
/*
 * 0: fit window
 * 1: fit width
 * 2: orginal size
 */
ImageFitMode Settings::imageFitMode() {
    int mode = settings->settingsConf->value("defaultFitMode", 0).toInt();
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
    settings->settingsConf->setValue("defaultFitMode", modeInt);
}
//------------------------------------------------------------------------------
QRect Settings::windowGeometry() {
    QRect savedRect = settings->stateConf->value("windowGeometry").toRect();
    if(savedRect.size().isEmpty())
        savedRect.setRect(100, 100, 900, 600);
    return savedRect;
}

void Settings::setWindowGeometry(QRect geometry) {
    settings->stateConf->setValue("windowGeometry", geometry);
}
//------------------------------------------------------------------------------
bool Settings::infiniteScrolling() {
    return settings->settingsConf->value("infiniteScrolling", false).toBool();
}

void Settings::setInfiniteScrolling(bool mode) {
    settings->settingsConf->setValue("infiniteScrolling", mode);
}
//------------------------------------------------------------------------------
void Settings::sendChangeNotification() {
    emit settingsChanged();
}
//------------------------------------------------------------------------------
void Settings::readShortcuts(QMap<QString, QString> &shortcuts) {
    settings->settingsConf->beginGroup("Controls");
    QStringList in, pair;
    in = settings->settingsConf->value("shortcuts").toStringList();
    for(int i = 0; i < in.count(); i++) {
        pair = in[i].split("=");
        if(!pair[0].isEmpty() && !pair[1].isEmpty()) {
            if(pair[1]=="eq")
                pair[1]="=";
            shortcuts.insert(pair[1], pair[0]);
        }
    }
    settings->settingsConf->endGroup();
}

void Settings::saveShortcuts(const QMap<QString, QString> &shortcuts) {
    settings->settingsConf->beginGroup("Controls");
    QMapIterator<QString, QString> i(shortcuts);
    QStringList out;
    while(i.hasNext()) {
        i.next();
        if(i.key() == "=")
            out << i.value() + "=" + "eq";
        else
            out << i.value() + "=" + i.key();
    }
    settings->settingsConf->setValue("shortcuts", out);
    settings->settingsConf->endGroup();
}
//------------------------------------------------------------------------------
void Settings::readScripts(QMap<QString, Script> &scripts) {
    scripts.clear();
    settings->settingsConf->beginGroup("Scripts");
    int size = settings->settingsConf->beginReadArray("script");
    for(int i=0; i < size; i++) {
        settings->settingsConf->setArrayIndex(i);
        QString name = settings->settingsConf->value("name").toString();
        QVariant value = settings->settingsConf->value("value");
        Script scr = value.value<Script>();
        scripts.insert(name, scr);
    }
    settings->settingsConf->endArray();
    settings->settingsConf->endGroup();
}

void Settings::saveScripts(const QMap<QString, Script> &scripts) {
    settings->settingsConf->beginGroup("Scripts");
    settings->settingsConf->beginWriteArray("script");
    QMapIterator<QString, Script> i(scripts);
    int counter = 0;
    while(i.hasNext()) {
        i.next();
        settings->settingsConf->setArrayIndex(counter);
        settings->settingsConf->setValue("name", i.key());
        settings->settingsConf->setValue("value", QVariant::fromValue(i.value()));
        counter++;
    }
    settings->settingsConf->endArray();
    settings->settingsConf->endGroup();
}
//------------------------------------------------------------------------------
bool Settings::squareThumbnails() {
    return settings->settingsConf->value("squareThumbnails", false).toBool();
}

void Settings::setSquareThumbnails(bool mode) {
    settings->settingsConf->setValue("squareThumbnails", mode);
}
//------------------------------------------------------------------------------
bool Settings::transparencyGrid() {
    return settings->settingsConf->value("drawTransparencyGrid", false).toBool();
}

void Settings::setTransparencyGrid(bool mode) {
    settings->settingsConf->setValue("drawTransparencyGrid", mode);
}
//------------------------------------------------------------------------------
bool Settings::enableSmoothScroll() {
    return settings->settingsConf->value("enableSmoothScroll", true).toBool();
}

void Settings::setEnableSmoothScroll(bool mode) {
    settings->settingsConf->setValue("enableSmoothScroll", mode);
}
//------------------------------------------------------------------------------
bool Settings::useThumbnailCache() {
    return settings->settingsConf->value("thumbnailCache", true).toBool();
}

void Settings::setUseThumbnailCache(bool mode) {
    settings->settingsConf->setValue("thumbnailCache", mode);
}
//------------------------------------------------------------------------------
QStringList Settings::savedPaths() {
    return settings->stateConf->value("savedPaths").toStringList();
}

void Settings::setSavedPaths(QStringList paths) {
    settings->stateConf->setValue("savedPaths", paths);
}
//------------------------------------------------------------------------------
QStringList Settings::bookmarks() {
    return settings->stateConf->value("bookmarks").toStringList();
}

void Settings::setBookmarks(QStringList paths) {
    settings->stateConf->setValue("bookmarks", paths);
}
//------------------------------------------------------------------------------
bool Settings::placesPanel() {
    return settings->stateConf->value("placesPanel", true).toBool();
}

void Settings::setPlacesPanel(bool mode) {
    settings->stateConf->setValue("placesPanel", mode);
}
//------------------------------------------------------------------------------
bool Settings::placesPanelBookmarksExpanded() {
    return settings->stateConf->value("placesPanelBookmarksExpanded", true).toBool();
}

void Settings::setPlacesPanelBookmarksExpanded(bool mode) {
    settings->stateConf->setValue("placesPanelBookmarksExpanded", mode);
}
//------------------------------------------------------------------------------
bool Settings::placesPanelTreeExpanded() {
    return settings->stateConf->value("placesPanelTreeExpanded", true).toBool();
}

void Settings::setPlacesPanelTreeExpanded(bool mode) {
    settings->stateConf->setValue("placesPanelTreeExpanded", mode);
}
//------------------------------------------------------------------------------
int Settings::placesPanelWidth() {
    return settings->stateConf->value("placesPanelWidth", 260).toInt();
}

void Settings::setPlacesPanelWidth(int width) {
    settings->stateConf->setValue("placesPanelWidth", width);
}
//------------------------------------------------------------------------------
void Settings::setSlideshowInterval(int ms) {
    settings->settingsConf->setValue("slideshowInterval", ms);
}

int Settings::slideshowInterval() {
    int interval = settings->settingsConf->value("slideshowInterval", 3000).toInt();
    if(interval <= 0)
        interval = 3000;
    return interval;
}
//------------------------------------------------------------------------------
int Settings::thumbnailerThreadCount() {
    int count = settings->settingsConf->value("thumbnailerThreads", 2).toInt();
    if(count < 1)
        count = 1;
    return count;
}

void Settings::setThumbnailerThreadCount(int count) {
    settings->settingsConf->setValue("thumbnailerThreads", count);
}
//------------------------------------------------------------------------------
bool Settings::smoothUpscaling() {
    return settings->settingsConf->value("smoothUpscaling", true).toBool();
}

void Settings::setSmoothUpscaling(bool mode) {
    settings->settingsConf->setValue("smoothUpscaling", mode);
}
//------------------------------------------------------------------------------
int Settings::folderViewIconSize() {
    return settings->settingsConf->value("folderViewIconSize", 150).toInt();
}

void Settings::setFolderViewIconSize(int value) {
    settings->settingsConf->setValue("folderViewIconSize", value);
}
//------------------------------------------------------------------------------
bool Settings::expandImage() {
    return settings->settingsConf->value("expandImage", false).toBool();
}

void Settings::setExpandImage(bool mode) {
    settings->settingsConf->setValue("expandImage", mode);
}
//------------------------------------------------------------------------------
int Settings::expandLimit() {
    return settings->settingsConf->value("expandLimit", 2).toInt();
}

void Settings::setExpandLimit(int value) {
    settings->settingsConf->setValue("expandLimit", value);
}
//------------------------------------------------------------------------------
int Settings::JPEGSaveQuality() {
    int quality = std::clamp(settings->settingsConf->value("JPEGSaveQuality", 95).toInt(), 0, 100);
    return quality;
}

void Settings::setJPEGSaveQuality(int value) {
    settings->settingsConf->setValue("JPEGSaveQuality", value);
}
//------------------------------------------------------------------------------
ScalingFilter Settings::scalingFilter() {
    int defaultFilter = 1;
#ifdef USE_OPENCV
    // default to a nicer QI_FILTER_CV_CUBIC
    defaultFilter = 3;
#endif
    int mode = settings->settingsConf->value("scalingFilter", defaultFilter).toInt();
#ifndef USE_OPENCV
    if(mode > 2)
        mode = 1;
#endif
    if(mode < 0 || mode > 4)
        mode = 1;
    return static_cast<ScalingFilter>(mode);
}

void Settings::setScalingFilter(ScalingFilter mode) {
    settings->settingsConf->setValue("scalingFilter", mode);
}
//------------------------------------------------------------------------------
bool Settings::smoothAnimatedImages() {
    return settings->settingsConf->value("smoothAnimatedImages", true).toBool();
}

void Settings::setSmoothAnimatedImages(bool mode) {
    settings->settingsConf->setValue("smoothAnimatedImages", mode);
}
//------------------------------------------------------------------------------
bool Settings::infoBarFullscreen() {
    return settings->settingsConf->value("infoBarFullscreen", true).toBool();
}

void Settings::setInfoBarFullscreen(bool mode) {
    settings->settingsConf->setValue("infoBarFullscreen", mode);
}
//------------------------------------------------------------------------------
bool Settings::infoBarWindowed() {
    return settings->settingsConf->value("infoBarWindowed", false).toBool();
}

void Settings::setInfoBarWindowed(bool mode) {
    settings->settingsConf->setValue("infoBarWindowed", mode);
}
//------------------------------------------------------------------------------
bool Settings::windowTitleExtendedInfo() {
    return settings->settingsConf->value("windowTitleExtendedInfo", true).toBool();
}

void Settings::setWindowTitleExtendedInfo(bool mode) {
    settings->settingsConf->setValue("windowTitleExtendedInfo", mode);
}

bool Settings::shuffleEnabled() {
    return settings->settingsConf->value("shuffleEnabled", false).toBool();
}

void Settings::setShuffleEnabled(bool mode) {
    settings->settingsConf->setValue("shuffleEnabled", mode);
}
//------------------------------------------------------------------------------
bool Settings::cursorAutohide() {
    return settings->settingsConf->value("cursorAutohiding", true).toBool();
}

void Settings::setCursorAutohide(bool mode) {
    settings->settingsConf->setValue("cursorAutohiding", mode);
}
//------------------------------------------------------------------------------
bool Settings::firstRun() {
    return settings->settingsConf->value("firstRun", true).toBool();
}

void Settings::setFirstRun(bool mode) {
    settings->settingsConf->setValue("firstRun", mode);
}
//------------------------------------------------------------------------------
bool Settings::useOpenGL() {
    // this causes several issues, ignore for now
    //return settings->s->value("useOpenGL", false).toBool();
    return false;
}

void Settings::setUseOpenGL(bool mode) {
    settings->settingsConf->setValue("useOpenGL", mode);
}
//------------------------------------------------------------------------------
qreal Settings::zoomStep() {
    bool ok = false;
    qreal value = settings->settingsConf->value("zoomStep", 0.2).toReal(&ok);
    if(!ok)
        return 0.5;
    if(value > 0.5)
        return 0.5;
    if(value < 0.1)
        return 0.1;
    return value;
}

void Settings::setZoomStep(qreal value) {
    if(value > 0.5)
        value = 0.5;
    else if(value < 0.1)
        value = 0.1;
    settings->settingsConf->setValue("zoomStep", value);
}
//------------------------------------------------------------------------------
void Settings::setZoomIndicatorMode(ZoomIndicatorMode mode) {
    settings->settingsConf->setValue("zoomIndicatorMode", mode);
}

ZoomIndicatorMode Settings::zoomIndicatorMode() {
    int mode = settings->settingsConf->value("zoomIndicatorMode", 0).toInt();
    if(mode < 0 || mode > 2)
        mode = 0;
    return static_cast<ZoomIndicatorMode>(mode);
}
//------------------------------------------------------------------------------
void Settings::setFocusPointIn1to1Mode(ImageFocusPoint mode) {
    settings->settingsConf->setValue("focusPointIn1to1Mode", mode);
}

ImageFocusPoint Settings::focusPointIn1to1Mode() {
    int mode = settings->settingsConf->value("focusPointIn1to1Mode", 1).toInt();
    if(mode < 0 || mode > 2)
        mode = 1;
    return static_cast<ImageFocusPoint>(mode);
}

void Settings::setDefaultCropAction(DefaultCropAction mode) {
    settings->settingsConf->setValue("defaultCropAction", mode);
}

DefaultCropAction Settings::defaultCropAction() {
    int mode = settings->settingsConf->value("defaultCropAction", 0).toInt();
    if(mode < 0 || mode > 1)
        mode = 0;
    return static_cast<DefaultCropAction>(mode);
}

ImageScrolling Settings::imageScrolling() {
    int mode = settings->settingsConf->value("imageScrolling", 1).toInt();
    if(mode < 0 || mode > 2)
        mode = 0;
    return static_cast<ImageScrolling>(mode);
}

void Settings::setImageScrolling(ImageScrolling mode) {
    settings->settingsConf->setValue("imageScrolling", mode);
}
//------------------------------------------------------------------------------
ViewMode Settings::defaultViewMode() {
    int mode = settings->settingsConf->value("defaultViewMode", 0).toInt();
    if(mode < 0 || mode > 1)
        mode = 0;
    return static_cast<ViewMode>(mode);
}

void Settings::setDefaultViewMode(ViewMode mode) {
    settings->settingsConf->setValue("defaultViewMode", mode);
}
