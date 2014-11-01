#include "settings.h"

Settings *globalSettings=NULL;

Settings::Settings(QObject *parent) :
    QObject(parent)
{
}

Settings* Settings::getInstance() {
    if (!globalSettings) globalSettings=new Settings();
        return globalSettings;
}

void Settings::sendChangeNotification() {
    emit settingsChanged();
}
