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
    fillVideoFormats();
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
        settings->loadTheme();
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
// this here is temporarily, will be moved to some sort of theme manager class
void Settings::loadStylesheet() {
    // stylesheet template file
    QFile file(":/res/styles/style-template.qss");
    if(file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());

        auto colors = settings->colorScheme();
        // for settings window
        QPalette p;
        // choose icons depending on text color
        /*if(p.text().color().valueF() > 0.5f) {
            mTheme.systemIconTheme = "light";
        } else {
            mTheme.systemIconTheme = "dark";
        } */
        QColor sys_window = p.window().color();
        QColor sys_window_tinted;
        if(sys_window.valueF() <= 0.45f) {
            sys_window_tinted.setHsv(sys_window.hue(), sys_window.saturation(), sys_window.value() + 16);
        } else {
            sys_window_tinted.setHsv(sys_window.hue(), sys_window.saturation(), sys_window.value() - 16);
        }

        // -------------- write variables into stylesheet ---------------
        //styleSheet.replace("%icontheme%",            settings->theme().iconTheme);
        styleSheet.replace("%icontheme%",            "light");
        styleSheet.replace("%button%",               colors.button.name());
        styleSheet.replace("%button_hover%",         colors.button_hover.name());
        styleSheet.replace("%button_pressed%",       colors.button_pressed.name());
        styleSheet.replace("%panel_button%",         colors.panel_button.name());
        styleSheet.replace("%panel_button_hover%",   colors.panel_button_hover.name());
        styleSheet.replace("%panel_button_pressed%", colors.panel_button_pressed.name());
        styleSheet.replace("%widget%",               colors.widget.name());
        styleSheet.replace("%widget_border%",        colors.widget_border.name());
        styleSheet.replace("%folderview%",           colors.folderview.name());
        styleSheet.replace("%folderview_topbar%",    colors.folderview_topbar.name());
        styleSheet.replace("%folderview_hc%",        colors.folderview_hc.name());
        styleSheet.replace("%folderview_hc2%",       colors.folderview_hc2.name());
        styleSheet.replace("%accent%",               colors.accent.name());
        styleSheet.replace("%input_field_focus%",    colors.input_field_focus.name());
        styleSheet.replace("%overlay%",              colors.overlay.name());
        styleSheet.replace("%icons%",                colors.icons.name());
        styleSheet.replace("%text_hc2%",             colors.text_hc2.name());
        styleSheet.replace("%text_hc%",              colors.text_hc.name());
        styleSheet.replace("%text%",                 colors.text.name());
        styleSheet.replace("%text_secondary_rgba%",  "rgba(" + QString::number(colors.text.red())   + ","
                                                             + QString::number(colors.text.green()) + ","
                                                             + QString::number(colors.text.blue())  + ",62%)");
        styleSheet.replace("%overlay_text%",         colors.overlay_text.name());
        styleSheet.replace("%text_lc%",              colors.text_lc.name());
        styleSheet.replace("%text_lc2%",             colors.text_lc2.name());
        styleSheet.replace("%scrollbar%",            colors.scrollbar.name());
        styleSheet.replace("%scrollbar_hover%",      colors.scrollbar_hover.name());
        styleSheet.replace("%system_window_tinted%", sys_window_tinted.name());
        styleSheet.replace("%folderview_button_hover%",   colors.folderview_button_hover.name());
        styleSheet.replace("%folderview_button_pressed%", colors.folderview_button_pressed.name());
        styleSheet.replace("%accent_hover_rgba%",    "rgba(" + QString::number(colors.accent.red())   + ","
                                                             + QString::number(colors.accent.green()) + ","
                                                             + QString::number(colors.accent.blue())  + ",65%)");
        styleSheet.replace("%overlay_rgba%",         "rgba(" + QString::number(colors.overlay.red())   + ","
                                                             + QString::number(colors.overlay.green()) + ","
                                                             + QString::number(colors.overlay.blue())  + ",90%)");
        styleSheet.replace("%fv_backdrop_rgba%",     "rgba(" + QString::number(colors.folderview_hc2.red())   + ","
                                                             + QString::number(colors.folderview_hc2.green()) + ","
                                                             + QString::number(colors.folderview_hc2.blue())  + ",80%)");
        if(colors.folderview != colors.folderview_topbar)
            styleSheet.replace("%topbar_border_rgba%", "rgba(0,0,0,14%)");
        else
            styleSheet.replace("%topbar_border_rgba%", colors.folderview.name());
        // Qt::Popup can't do transparency under windows, use square window
#ifdef _WIN32
        styleSheet.replace("%contextmenu_border_radius%",  "0px");
#else
        styleSheet.replace("%contextmenu_border_radius%",  "3px");
#endif
        // ------------------------ apply ----------------------------
        qApp->setStyleSheet(styleSheet);
    }
}
//------------------------------------------------------------------------------
void Settings::loadTheme() {
    if(settings->useSystemColorScheme()) {
        setColorScheme(ThemeStore::colorScheme(ColorSchemes::COLORS_SYSTEM));
    } else {
        BaseColorScheme base;
        themeConf->beginGroup("Colors");
        base.background            = QColor(themeConf->value("background",            "#1a1a1a").toString());
        base.background_fullscreen = QColor(themeConf->value("background_fullscreen", "#1a1a1a").toString());
        base.text                  = QColor(themeConf->value("text",                  "#b6b6b6").toString());
        base.icons                 = QColor(themeConf->value("icons",                 "#a4a4a4").toString());
        base.widget                = QColor(themeConf->value("widget",                "#252525").toString());
        base.widget_border         = QColor(themeConf->value("widget_border",         "#2c2c2c").toString());
        base.accent                = QColor(themeConf->value("accent",                "#8c9b81").toString());
        base.folderview            = QColor(themeConf->value("folderview",            "#242424").toString());
        base.folderview_topbar     = QColor(themeConf->value("folderview_topbar",     "#383838").toString());
        base.scrollbar             = QColor(themeConf->value("scrollbar",             "#5a5a5a").toString());
        base.overlay_text          = QColor(themeConf->value("overlay_text",          "#d2d2d2").toString());
        base.overlay               = QColor(themeConf->value("overlay",               "#1a1a1a").toString());
        themeConf->endGroup();
        setColorScheme(ColorScheme(base));
    }
}
void Settings::saveTheme() {
    if(settings->useSystemColorScheme())
        return;
    themeConf->beginGroup("Colors");
    themeConf->setValue("background",            mColorScheme.background.name());
    themeConf->setValue("background_fullscreen", mColorScheme.background_fullscreen.name());
    themeConf->setValue("text",                  mColorScheme.text.name());
    themeConf->setValue("icons",                 mColorScheme.icons.name());
    themeConf->setValue("widget",                mColorScheme.widget.name());
    themeConf->setValue("widget_border",         mColorScheme.widget_border.name());
    themeConf->setValue("accent",                mColorScheme.accent.name());
    themeConf->setValue("folderview",            mColorScheme.folderview.name());
    themeConf->setValue("folderview_topbar",     mColorScheme.folderview_topbar.name());
    themeConf->setValue("scrollbar",             mColorScheme.scrollbar.name());
    themeConf->setValue("overlay_text",          mColorScheme.overlay_text.name());
    themeConf->setValue("overlay",               mColorScheme.overlay.name());
    themeConf->endGroup();
}
//------------------------------------------------------------------------------
const ColorScheme& Settings::colorScheme() {
    return mColorScheme;
}
//------------------------------------------------------------------------------
void Settings::setColorScheme(ColorScheme scheme) {
    mColorScheme = scheme;
    loadStylesheet();
}
//------------------------------------------------------------------------------
void Settings::fillVideoFormats() {
    mVideoFormatsMap.insert("video/webm",       "webm");
    mVideoFormatsMap.insert("video/mp4",        "mp4");
    mVideoFormatsMap.insert("video/mpeg",       "mpg");
    mVideoFormatsMap.insert("video/mpeg",       "mpeg");
    mVideoFormatsMap.insert("video/x-matroska", "mkv");
    mVideoFormatsMap.insert("video/x-ms-wmv",   "wmv");
    mVideoFormatsMap.insert("video/x-msvideo",  "avi");
    mVideoFormatsMap.insert("video/quicktime",  "mov");
    mVideoFormatsMap.insert("video/x-flv",      "flv");
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
        if(!QFile::exists(mpvPath))
            mpvPath = "";
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
        formats << mVideoFormatsMap.values();
    formats.removeAll("pdf");
    return formats;
}
//------------------------------------------------------------------------------
// (for open/save dialogs, as a single string)
// example:  "Images (*.jpg, *.png)"
QString Settings::supportedFormatsFilter() {
    QString filters;
    auto formats = supportedFormats();
    filters.append("Supported files (");
    for(int i = 0; i < formats.count(); i++)
        filters.append("*." + QString(formats.at(i)) + " ");
    filters.append(")");
    return filters;
}
//------------------------------------------------------------------------------
QString Settings::supportedFormatsRegex() {
    QString filter;
    QList<QByteArray> formats = supportedFormats();
    filter.append(".*\\.(");
    for(int i = 0; i < formats.count(); i++)
        filter.append(QString(formats.at(i)) + "|");
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
        mimeTypes << mVideoFormatsMap.keys();
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
bool Settings::useSystemColorScheme() {
    return settings->settingsConf->value("useSystemColorScheme", false).toBool();
}

void Settings::setUseSystemColorScheme(bool mode) {
    settings->settingsConf->setValue("useSystemColorScheme", mode);
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
FolderViewMode Settings::folderViewMode() {
    int mode = settings->settingsConf->value("folderViewMode", 2).toInt();
    if(mode < 0 || mode >= 3)
        mode = 2;
    return static_cast<FolderViewMode>(mode);
}

void Settings::setFolderViewMode(FolderViewMode mode) {
    settings->settingsConf->setValue("folderViewMode", mode);
}
//------------------------------------------------------------------------------
ThumbPanelStyle Settings::thumbPanelStyle() {
    int mode = settings->settingsConf->value("thumbPanelStyle", 1).toInt();
    if(mode < 0 || mode > 1)
        mode = 1;
    return static_cast<ThumbPanelStyle>(mode);
}

void Settings::setThumbPanelStyle(ThumbPanelStyle mode) {
    settings->settingsConf->setValue("thumbPanelStyle", mode);
}
//------------------------------------------------------------------------------
const QMultiMap<QByteArray, QByteArray> Settings::videoFormats() const {
    return mVideoFormatsMap;
}
//------------------------------------------------------------------------------
int Settings::panelPreviewsSize() {
    bool ok = true;
    int size = settings->settingsConf->value("panelPreviewsSize", 140).toInt(&ok);
    if(!ok)
        size = 140;
    size = qBound(100, size, 250);
    return size;
}

void Settings::setPanelPreviewsSize(int size) {
    settings->settingsConf->setValue("panelPreviewsSize", size);
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
bool Settings::loopSlideshow() {
    return settings->settingsConf->value("loopSlideshow", false).toBool();
}

void Settings::setLoopSlideshow(bool mode) {
    settings->settingsConf->setValue("loopSlideshow", mode);
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
    return settings->settingsConf->value("enableSmoothScroll", false).toBool();
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
    int count = settings->settingsConf->value("thumbnailerThreads", 4).toInt();
    if(count < 1)
        count = 4;
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
    return settings->settingsConf->value("folderViewIconSize", 120).toInt();
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
bool Settings::showSaveOverlay() {
    return settings->settingsConf->value("showSaveOverlay", true).toBool();
}

void Settings::setShowSaveOverlay(bool mode) {
    settings->settingsConf->setValue("showSaveOverlay", mode);
}
//------------------------------------------------------------------------------
bool Settings::confirmDelete() {
    return settings->settingsConf->value("confirmDelete", true).toBool();
}

void Settings::setConfirmDelete(bool mode) {
    settings->settingsConf->setValue("confirmDelete", mode);
}
//------------------------------------------------------------------------------
bool Settings::confirmTrash() {
    return settings->settingsConf->value("confirmTrash", true).toBool();
}

void Settings::setConfirmTrash(bool mode) {
    settings->settingsConf->setValue("confirmTrash", mode);
}
//------------------------------------------------------------------------------
bool Settings::unloadThumbs() {
    return settings->settingsConf->value("unloadThumbs", true).toBool();
}

void Settings::setUnloadThumbs(bool mode) {
    settings->settingsConf->setValue("unloadThumbs", mode);
}
//------------------------------------------------------------------------------
qreal Settings::zoomStep() {
    bool ok = false;
    qreal value = settings->settingsConf->value("zoomStep", 0.2).toReal(&ok);
    if(!ok)
        return 0.2;
    value = qBound(0.01, value, 0.5);
    return value;
}

void Settings::setZoomStep(qreal value) {
    value = qBound(0.01, value, 0.5);
    settings->settingsConf->setValue("zoomStep", value);
}
//------------------------------------------------------------------------------
bool Settings::absoluteZoomStep() {
    bool mode = settings->settingsConf->value("absoluteZoomStep", false).toBool();
    return mode;
}

void Settings::setAbsoluteZoomStep(bool mode) {
    settings->settingsConf->setValue("absoluteZoomStep", mode);
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
//------------------------------------------------------------------------------
FolderEndAction Settings::folderEndAction() {
    int mode = settings->settingsConf->value("folderEndAction", 0).toInt();
    if(mode < 0 || mode > 2)
        mode = 0;
    return static_cast<FolderEndAction>(mode);
}

void Settings::setFolderEndAction(FolderEndAction mode) {
    settings->settingsConf->setValue("folderEndAction", mode);
}
//------------------------------------------------------------------------------
bool Settings::printLandscape() {
    return stateConf->value("printLandscape", false).toBool();
}

void Settings::setPrintLandscape(bool mode) {
    stateConf->setValue("printLandscape", mode);
}
//------------------------------------------------------------------------------
bool Settings::printPdfDefault() {
    return stateConf->value("printPdfDefault", false).toBool();
}

void Settings::setPrintPdfDefault(bool mode) {
    stateConf->setValue("printPdfDefault", mode);
}
//------------------------------------------------------------------------------
bool Settings::printColor() {
    return stateConf->value("printColor", false).toBool();
}

void Settings::setPrintColor(bool mode) {
    stateConf->setValue("printColor", mode);
}
//------------------------------------------------------------------------------
bool Settings::printFitToPage() {
    return stateConf->value("printFitToPage", true).toBool();
}

void Settings::setPrintFitToPage(bool mode) {
    stateConf->setValue("printFitToPage", mode);
}
//------------------------------------------------------------------------------
QString Settings::lastPrinter() {
    return stateConf->value("lastPrinter", "").toString();
}

void Settings::setLastPrinter(QString name) {
    stateConf->setValue("lastPrinter", name);
}
//------------------------------------------------------------------------------
bool Settings::jxlAnimation() {
    return settings->settingsConf->value("jxlAnimation", false).toBool();
}

void Settings::setJxlAnimation(bool mode) {
    settings->settingsConf->setValue("jxlAnimation", mode);
}
//------------------------------------------------------------------------------
bool Settings::autoResizeWindow() {
    return settings->settingsConf->value("autoResizeWindow", false).toBool();
}

void Settings::setAutoResizeWindow(bool mode) {
    settings->settingsConf->setValue("autoResizeWindow", mode);
}
//------------------------------------------------------------------------------
int Settings::autoResizeLimit() {
    int limit = settings->settingsConf->value("autoResizeLimit", 90).toInt();
    if(limit < 30 || limit > 100)
        limit = 90;
    return limit;
}

void Settings::setAutoResizeLimit(int percent) {
    settings->settingsConf->setValue("autoResizeLimit", percent);
}
