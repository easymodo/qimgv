#include "actions.h"

Actions *appActions = nullptr;

Actions::Actions() {
    init();
}

Actions *Actions::getInstance() {
    if(!appActions)
        appActions  = new Actions();
    return appActions;
}

const QMap<QString, QVersionNumber> &Actions::getMap() {
    return mActions;
}

QList<QString> Actions::getList() {
    return mActions.keys();
}

void Actions::init() {
    mActions.insert("nextImage", QVersionNumber(0,6,2));
    mActions.insert("prevImage", QVersionNumber(0,6,2));
    mActions.insert("toggleFullscreen", QVersionNumber(0,6,2));
    mActions.insert("fitWindow", QVersionNumber(0,6,2));
    mActions.insert("fitWidth", QVersionNumber(0,6,2));
    mActions.insert("fitNormal", QVersionNumber(0,6,2));
    mActions.insert("toggleFitMode", QVersionNumber(0,6,2));
    mActions.insert("resize", QVersionNumber(0,6,2));
    mActions.insert("rotateRight", QVersionNumber(0,6,2));
    mActions.insert("rotateLeft", QVersionNumber(0,6,2));
    mActions.insert("scrollUp", QVersionNumber(0,6,2));
    mActions.insert("scrollDown", QVersionNumber(0,6,2));
    mActions.insert("scrollLeft", QVersionNumber(0,6,2));
    mActions.insert("scrollRight", QVersionNumber(0,6,2));
    mActions.insert("zoomIn", QVersionNumber(0,6,2));
    mActions.insert("zoomOut", QVersionNumber(0,6,2));
    mActions.insert("zoomInCursor", QVersionNumber(0,6,2));
    mActions.insert("zoomOutCursor", QVersionNumber(0,6,2));
    mActions.insert("open", QVersionNumber(0,6,2));
    mActions.insert("save", QVersionNumber(0,6,2));
    mActions.insert("saveAs", QVersionNumber(0,6,2));
    mActions.insert("setWallpaper", QVersionNumber(0,9,3));
    mActions.insert("crop", QVersionNumber(0,6,2));
    mActions.insert("removeFile", QVersionNumber(0,6,2));
    mActions.insert("copyFile", QVersionNumber(0,6,2));
    mActions.insert("moveFile", QVersionNumber(0,6,2));
    mActions.insert("jumpToFirst", QVersionNumber(0,6,2));
    mActions.insert("jumpToLast", QVersionNumber(0,6,2));
    mActions.insert("openSettings", QVersionNumber(0,6,2));
    mActions.insert("closeFullScreenOrExit", QVersionNumber(0,6,2));
    mActions.insert("exit", QVersionNumber(0,6,2));
    mActions.insert("flipH", QVersionNumber(0,6,3));
    mActions.insert("flipV", QVersionNumber(0,6,3));
    mActions.insert("folderView", QVersionNumber(0,6,85));
    mActions.insert("pauseVideo", QVersionNumber(0,6,85));
    mActions.insert("frameStep", QVersionNumber(0,6,85));
    mActions.insert("frameStepBack", QVersionNumber(0,6,85));
    mActions.insert("folderView", QVersionNumber(0,6,85));
    mActions.insert("documentView", QVersionNumber(0,6,88));
    mActions.insert("toggleFolderView", QVersionNumber(0,6,88));
    mActions.insert("moveToTrash", QVersionNumber(0,6,89));
    mActions.insert("reloadImage", QVersionNumber(0,7,80));
    mActions.insert("copyFileClipboard", QVersionNumber(0,7,80));
    mActions.insert("copyPathClipboard", QVersionNumber(0,7,80));
    mActions.insert("renameFile", QVersionNumber(0,7,80));
    mActions.insert("contextMenu", QVersionNumber(0,7,81));
    mActions.insert("toggleTransparencyGrid", QVersionNumber(0,7,82));
    mActions.insert("sortByName", QVersionNumber(0,7,83));
    mActions.insert("sortByTime", QVersionNumber(0,7,83));
    mActions.insert("sortBySize", QVersionNumber(0,7,83));
    mActions.insert("toggleImageInfo", QVersionNumber(0,7,84));
    mActions.insert("toggleShuffle", QVersionNumber(0,8,3));
    mActions.insert("toggleScalingFilter", QVersionNumber(0,8,3));
    mActions.insert("toggleMute", QVersionNumber(0,8,7));
    mActions.insert("volumeUp", QVersionNumber(0,8,7));
    mActions.insert("volumeDown", QVersionNumber(0,8,7));
    mActions.insert("toggleSlideshow", QVersionNumber(0,8,81));
    mActions.insert("showInDirectory", QVersionNumber(0,8,82));
    mActions.insert("goUp", QVersionNumber(0,9,2));
    mActions.insert("discardEdits", QVersionNumber(0,9,2));
    mActions.insert("nextDirectory", QVersionNumber(0,9,2));
    mActions.insert("prevDirectory", QVersionNumber(0,9,2));
    mActions.insert("seekVideoForward", QVersionNumber(0,9,2));
    mActions.insert("seekVideoBackward", QVersionNumber(0,9,2));
    mActions.insert("lockZoom", QVersionNumber(0,9,2));
    mActions.insert("lockView", QVersionNumber(0,9,2));
    mActions.insert("print", QVersionNumber(1,0,0));
    mActions.insert("toggleFullscreenInfoBar", QVersionNumber(1,0,0));
    mActions.insert("pasteFile", QVersionNumber(1,0,3));
}

