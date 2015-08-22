#include "settings.h"

Settings *globalSettings=NULL;

Settings::Settings(QObject *parent) :
    QObject(parent)
{
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
        if(globalSettings->s.value("lastDir") == "") {
            globalSettings->s.setValue("lastDir",
                                       QApplication::applicationDirPath());
        }
        // minimum cache size
        if(globalSettings->s.value("cacheSize").toInt() < 32) {
            globalSettings->s.setValue("cacheSize","32");
        }
    }
}

void Settings::sendChangeNotification() {
    emit settingsChanged();
}

