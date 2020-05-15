#include "settings.h"

Settings *settings = nullptr;

Settings::Settings(QObject *parent) : QObject(parent) {
#ifdef __linux__
    QString genericCacheLocation = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    if(genericCacheLocation.isEmpty())
        genericCacheLocation = QDir::homePath() + "/.cache";
    mTmpDir = new QDir(genericCacheLocation + "/" + QApplication::applicationName());
    mTmpDir->mkpath(mTmpDir->absolutePath());
    mThumbCacheDir = new QDir(mTmpDir->absolutePath() + "/thumbnails");
    mThumbCacheDir->mkpath(mThumbCacheDir->absolutePath());
    QSettings::setDefaultFormat(QSettings::NativeFormat);
    s = new QSettings();
    state = new QSettings(QCoreApplication::organizationName(), "savedState");
    theme = new QSettings(QCoreApplication::organizationName(), "theme");
#else
    mTmpDir = new QDir(QApplication::applicationDirPath() + "/cache");
    mTmpDir->mkpath(mTmpDir->absolutePath());
    mThumbCacheDir = new QDir(QApplication::applicationDirPath() + "/thumbnails");
    mThumbCacheDir->mkpath(mThumbCacheDir->absolutePath());
    mConfDir = new QDir(QApplication::applicationDirPath() + "/conf");
    mConfDir->mkpath(QApplication::applicationDirPath() + "/conf");
    s = new QSettings(mConfDir->absolutePath() + "/" + qApp->applicationName() + ".ini", QSettings::IniFormat);
    state = new QSettings(mConfDir->absolutePath() + "/savedState.ini", QSettings::IniFormat);
    theme = new QSettings(mConfDir->absolutePath() + "/theme.ini", QSettings::IniFormat);
#endif
    loadColorScheme();
}
//------------------------------------------------------------------------------
Settings::~Settings() {
    saveColorScheme();
    delete mThumbCacheDir;
    delete mTmpDir;
    delete s;
    delete state;
    delete theme;
}
//------------------------------------------------------------------------------
Settings *Settings::getInstance() {
    if(!settings)
        settings = new Settings();
    return settings;
}
//------------------------------------------------------------------------------
void Settings::sync() {
    settings->s->sync();
    settings->state->sync();
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
void Settings::loadColorScheme() {
    mColorScheme.background            = QColor(theme->value("background",            "#1b1b1c").toString());
    mColorScheme.background_fullscreen = QColor(theme->value("background_fullscreen", "#1b1b1c").toString());
    mColorScheme.text                  = QColor(theme->value("text",                  "#9c9ea0").toString());
    mColorScheme.widget                = QColor(theme->value("widget",                "#232324").toString());
    mColorScheme.widget_border         = QColor(theme->value("widget_border",         "#232324").toString());
    mColorScheme.button                = QColor(theme->value("button",                "#2f2f30").toString());
    mColorScheme.accent                = QColor(theme->value("accent",                "#375e87").toString());
    mColorScheme.folderview            = QColor(theme->value("folderview",            "#2f2f30").toString());
    mColorScheme.folderview_topbar     = QColor(theme->value("folderview_topbar",     "#1a1a1b").toString());
    mColorScheme.folderview_panel      = QColor(theme->value("folderview_panel",      "#373738").toString());
    mColorScheme.slider_groove         = QColor(theme->value("slider_groove",         "#2f2f30").toString());
    mColorScheme.slider_handle         = QColor(theme->value("slider_handle",         "#5c5e60").toString());
    mColorScheme.overlay_text          = QColor(theme->value("overlay_text",          "#d2d2d2").toString());
    mColorScheme.overlay               = QColor(theme->value("overlay",               "#1a1a1b").toString());
    createColorVariants();
}
void Settings::saveColorScheme() {
    theme->beginGroup("Colors");
    theme->setValue("background",            mColorScheme.background.name());
    theme->setValue("background_fullscreen", mColorScheme.background_fullscreen.name());
    theme->setValue("text",                  mColorScheme.text.name());
    theme->setValue("widget",                mColorScheme.widget.name());
    theme->setValue("widget_border",         mColorScheme.widget_border.name());
    theme->setValue("button",                mColorScheme.button.name());
    theme->setValue("accent",                mColorScheme.accent.name());
    theme->setValue("folderview",            mColorScheme.folderview.name());
    theme->setValue("folderview_topbar",     mColorScheme.folderview_topbar.name());
    theme->setValue("folderview_panel",      mColorScheme.folderview_panel.name());
    theme->setValue("slider_groove",         mColorScheme.slider_groove.name());
    theme->setValue("slider_handle",         mColorScheme.slider_handle.name());
    theme->setValue("overlay_text",          mColorScheme.overlay_text.name());
    theme->setValue("overlay",               mColorScheme.overlay.name());
    theme->endGroup();
}
//------------------------------------------------------------------------------
void Settings::createColorVariants() {
    // darker & desaturated
    mColorScheme.accent_darker.setHsv(mColorScheme.accent.hue(),
                                      mColorScheme.accent.saturation() * 0.8f,
                                      mColorScheme.accent.value() * 0.7f);
    mColorScheme.button_hover      = QColor(mColorScheme.button.lighter(115));
    mColorScheme.button_pressed    = QColor(mColorScheme.button.darker(120));
    mColorScheme.button_border     = QColor(mColorScheme.button.darker(145));
    mColorScheme.input_field_focus = QColor(mColorScheme.accent_darker);
    mColorScheme.slider_hover      = QColor(mColorScheme.slider_handle.lighter(120));
    mColorScheme.text_lighter      = QColor(mColorScheme.text.lighter(120));
    mColorScheme.text_light        = QColor(mColorScheme.text.lighter(110));
    mColorScheme.text_dark         = QColor(mColorScheme.text.darker(104));
    mColorScheme.text_darker       = QColor(mColorScheme.text.darker(112));
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
QList<QByteArray> Settings::supportedFormats() {
    auto formats = QImageReader::supportedImageFormats();
    formats << "jfif";
    if(videoPlayback())
        formats << "webm" << "mp4";
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
    return settings->s->value("videoPlayback", true).toBool();
#else
    return false;
#endif
}

void Settings::setVideoPlayback(bool mode) {
    settings->s->setValue("videoPlayback", mode);
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
        mode = SortingMode::SORT_NAME;
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
void Settings::setVolume(int vol) {
    settings->state->setValue("volume", vol);
}

int Settings::volume() {
    return settings->state->value("volume", 100).toInt();
}
//------------------------------------------------------------------------------
bool Settings::showThumbnailLabels() {
    return settings->s->value("showThumbnailLabels", false).toBool();
}

void Settings::setShowThumbnailLabels(bool mode) {
    settings->s->setValue("showThumbnailLabels", mode);
}
//------------------------------------------------------------------------------
int Settings::mainPanelSize() {
    bool ok = true;
    int size = settings->s->value("mainPanelSize", mainPanelSizeDefault).toInt(&ok);
    if(!ok)
        size = mainPanelSizeDefault;
    size = qBound(160, size, 350);
    return size;
}

void Settings::setMainPanelSize(int size) {
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
bool Settings::keepFitMode() {
    return settings->s->value("keepFitMode", false).toBool();
}

void Settings::setKeepFitMode(bool mode) {
    settings->s->setValue("keepFitMode", mode);
}
//------------------------------------------------------------------------------
bool Settings::fullscreenMode() {
    return settings->s->value("openInFullscreen", false).toBool();
}

void Settings::setFullscreenMode(bool mode) {
    settings->s->setValue("openInFullscreen", mode);
}
//------------------------------------------------------------------------------
bool Settings::maximizedWindow() {
    return settings->state->value("maximizedWindow", false).toBool();
}

void Settings::setMaximizedWindow(bool mode) {
    settings->state->setValue("maximizedWindow", mode);
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
    return settings->s->value("panelFullscreenOnly", true).toBool();
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
bool Settings::squareThumbnails() {
    return settings->s->value("squareThumbnails", false).toBool();
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
QStringList Settings::bookmarks() {
    return settings->state->value("bookmarks").toStringList();
}

void Settings::setBookmarks(QStringList paths) {
    settings->state->setValue("bookmarks", paths);
}
//------------------------------------------------------------------------------
bool Settings::placesPanel() {
    return settings->state->value("placesPanel", true).toBool();
}

void Settings::setPlacesPanel(bool mode) {
    settings->state->setValue("placesPanel", mode);
}
//------------------------------------------------------------------------------
bool Settings::placesPanelBookmarksExpanded() {
    return settings->state->value("placesPanelBookmarksExpanded", true).toBool();
}

void Settings::setPlacesPanelBookmarksExpanded(bool mode) {
    settings->state->setValue("placesPanelBookmarksExpanded", mode);
}
//------------------------------------------------------------------------------
bool Settings::placesPanelTreeExpanded() {
    return settings->state->value("placesPanelTreeExpanded", true).toBool();
}

void Settings::setPlacesPanelTreeExpanded(bool mode) {
    settings->state->setValue("placesPanelTreeExpanded", mode);
}
//------------------------------------------------------------------------------
int Settings::placesPanelWidth() {
    return settings->state->value("placesPanelWidth", 260).toInt();
}

void Settings::setPlacesPanelWidth(int width) {
    settings->state->setValue("placesPanelWidth", width);
}
//------------------------------------------------------------------------------
void Settings::setSlideshowInterval(int ms) {
    settings->s->setValue("slideshowInterval", ms);
}

int Settings::slideshowInterval() {
    int interval = settings->s->value("slideshowInterval", 3000).toInt();
    if(interval <= 0)
        interval = 3000;
    return interval;
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
int Settings::folderViewIconSize() {
    return settings->s->value("folderViewIconSize", 150).toInt();
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
int Settings::expandLimit() {
    return settings->s->value("expandLimit", 2).toInt();
}

void Settings::setExpandLimit(int value) {
    settings->s->setValue("expandLimit", value);
}
//------------------------------------------------------------------------------
int Settings::JPEGSaveQuality() {
    int quality = std::clamp(settings->s->value("JPEGSaveQuality", 95).toInt(), 0, 100);
    return quality;
}

void Settings::setJPEGSaveQuality(int value) {
    settings->s->setValue("JPEGSaveQuality", value);
}
//------------------------------------------------------------------------------
ScalingFilter Settings::scalingFilter() {
    int defaultFilter = 1;
#ifdef USE_OPENCV
    // default to a nicer QI_FILTER_CV_CUBIC
    defaultFilter = 3;
#endif
    int mode = settings->s->value("scalingFilter", defaultFilter).toInt();
#ifndef USE_OPENCV
    if(mode > 2)
        mode = 1;
#endif
    if(mode < 0 || mode > 4)
        mode = 1;
    return static_cast<ScalingFilter>(mode);
}

void Settings::setScalingFilter(ScalingFilter mode) {
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
bool Settings::infoBarFullscreen() {
    return settings->s->value("infoBarFullscreen", true).toBool();
}

void Settings::setInfoBarFullscreen(bool mode) {
    settings->s->setValue("infoBarFullscreen", mode);
}
//------------------------------------------------------------------------------
bool Settings::infoBarWindowed() {
    return settings->s->value("infoBarWindowed", false).toBool();
}

void Settings::setInfoBarWindowed(bool mode) {
    settings->s->setValue("infoBarWindowed", mode);
}
//------------------------------------------------------------------------------
bool Settings::windowTitleExtendedInfo() {
    return settings->s->value("windowTitleExtendedInfo", true).toBool();
}

void Settings::setWindowTitleExtendedInfo(bool mode) {
    settings->s->setValue("windowTitleExtendedInfo", mode);
}

bool Settings::shuffleEnabled() {
    return settings->s->value("shuffleEnabled", false).toBool();
}

void Settings::setShuffleEnabled(bool mode) {
    settings->s->setValue("shuffleEnabled", mode);
}
//------------------------------------------------------------------------------
bool Settings::cursorAutohide() {
    return settings->s->value("cursorAutohiding", true).toBool();
}

void Settings::setCursorAutohide(bool mode) {
    settings->s->setValue("cursorAutohiding", mode);
}
//------------------------------------------------------------------------------
bool Settings::firstRun() {
    return settings->s->value("firstRun", true).toBool();
}

void Settings::setFirstRun(bool mode) {
    settings->s->setValue("firstRun", mode);
}
//------------------------------------------------------------------------------
bool Settings::useOpenGL() {
    return settings->s->value("useOpenGL", false).toBool();
}

void Settings::setUseOpenGL(bool mode) {
    settings->s->setValue("useOpenGL", mode);
}
//------------------------------------------------------------------------------
qreal Settings::zoomStep() {
    bool ok = false;
    qreal value = settings->s->value("zoomStep", 0.2).toReal(&ok);
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
    settings->s->setValue("zoomStep", value);
}
//------------------------------------------------------------------------------
void Settings::setZoomIndicatorMode(ZoomIndicatorMode mode) {
    settings->s->setValue("zoomIndicatorMode", mode);
}

ZoomIndicatorMode Settings::zoomIndicatorMode() {
    int mode = settings->s->value("zoomIndicatorMode", 0).toInt();
    if(mode < 0 || mode > 2)
        mode = 0;
    return static_cast<ZoomIndicatorMode>(mode);
}
//------------------------------------------------------------------------------
void Settings::setFocusPointIn1to1Mode(ImageFocusPoint mode) {
    settings->s->setValue("focusPointIn1to1Mode", mode);
}

ImageFocusPoint Settings::focusPointIn1to1Mode() {
    int mode = settings->s->value("focusPointIn1to1Mode", 1).toInt();
    if(mode < 0 || mode > 2)
        mode = 1;
    return static_cast<ImageFocusPoint>(mode);
}

void Settings::setDefaultCropAction(DefaultCropAction mode) {
    settings->s->setValue("defaultCropAction", mode);
}

DefaultCropAction Settings::defaultCropAction() {
    int mode = settings->s->value("defaultCropAction", 0).toInt();
    if(mode < 0 || mode > 1)
        mode = 0;
    return static_cast<DefaultCropAction>(mode);
}

ImageScrolling Settings::imageScrolling() {
    int mode = settings->s->value("imageScrolling", 1).toInt();
    if(mode < 0 || mode > 2)
        mode = 0;
    return static_cast<ImageScrolling>(mode);
}

void Settings::setImageScrolling(ImageScrolling mode) {
    settings->s->setValue("imageScrolling", mode);
}
//------------------------------------------------------------------------------
ViewMode Settings::defaultViewMode() {
    int mode = settings->s->value("defaultViewMode", 0).toInt();
    if(mode < 0 || mode > 1)
        mode = 0;
    return static_cast<ViewMode>(mode);
}

void Settings::setDefaultViewMode(ViewMode mode) {
    settings->s->setValue("defaultViewMode", mode);
}

const ColorScheme& Settings::colorScheme() {
    return mColorScheme;
}

void Settings::setColorScheme(ColorScheme &scheme) {
    mColorScheme = scheme;
    createColorVariants();
    saveColorScheme();
}
