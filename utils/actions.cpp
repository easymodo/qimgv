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
    mActions.insert("setWallpaper", QVersionNumber(0,6,2));
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
    mActions.insert("seekVideo", QVersionNumber(0,6,85));
    mActions.insert("seekBackVideo", QVersionNumber(0,6,85));
    mActions.insert("frameStep", QVersionNumber(0,6,85));
    mActions.insert("frameStepBack", QVersionNumber(0,6,85));
    mActions.insert("folderView", QVersionNumber(0,6,85));
    mActions.insert("documentView", QVersionNumber(0,6,88));
    mActions.insert("toggleFolderView", QVersionNumber(0,6,88));
    mActions.insert("moveToTrash", QVersionNumber(0,6,89));
}

