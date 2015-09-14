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

//returns list of regexps
QStringList Settings::supportedFormats() {
    QStringList filters;
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    for(int i = 0; i < supportedFormats.count(); i++) {
        filters << "*."+QString(supportedFormats.at(i));
    }
    return filters;
}

// as a single string (for open/save dialogs)
QString Settings::supportedFormatsString() {
    QString filters;
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    filters.append("Images (");
    for(int i = 0; i < supportedFormats.count(); i++) {
        filters.append("*."+QString(supportedFormats.at(i))+" ");
    }
    filters.append(")");
    return filters;
}

/* 0: By name (default)
 * 1: By name reversed
 * 2: By date
 * 3: By date reversed
*/
int Settings::sortingMode() {
    bool ok = true;
    int mode = this->s.value("sortingMode", "0").toInt(&ok);
    if(!ok) {
        mode = 0;
    }
    return mode;
}

bool Settings::setSortingMode(int mode) {
    if(mode < 0 || mode > 3) {
        qDebug() << "invalid sorting mode (" << mode << "), resetting to default.";
        mode = 0;
    }
    s.setValue("sortingMode", mode);
}

void Settings::sendChangeNotification() {
    emit settingsChanged();
}

