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
        bool ok = true;
        if(globalSettings->s.value("lastDir") == "") {
            globalSettings->s.setValue("lastDir",
                                       QApplication::applicationDirPath());
        }
        // minimum cache size
        if(globalSettings->s.value("cacheSize").toInt() < 32) {
            globalSettings->s.setValue("cacheSize","32");
        }
        if(ok!=true) {
            qDebug() << "Settings: error reading thumbnail size (int conversion failed).";
            qDebug() << "Settings: setting default size.";
            ok = true;
            globalSettings->s.setValue("thumbnailSize", "120");
        }
    }
}

void Settings::sendChangeNotification() {
    emit settingsChanged();
}

