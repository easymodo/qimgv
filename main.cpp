#include "core.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "settings.h"

void saveSettings() {
    delete globalSettings;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("greenpepper software");
    QCoreApplication::setOrganizationDomain("github.com/easymodo/qimgv");
    QCoreApplication::setApplicationName("qimgv");
    QCoreApplication::setApplicationVersion("0.35.1");

    globalSettings = Settings::getInstance();
    atexit(saveSettings);

    QFile file(":/styles/res/style.qss");
    if(file.open(QFile::ReadOnly)) {
       QString StyleSheet = QLatin1String(file.readAll());
       qApp->setStyleSheet(StyleSheet);
    }

    MainWindow mw;
    if( globalSettings->fullscreenMode() ) {
        mw.slotTriggerFullscreen();
    }
    if(a.arguments().length()>1) {
        QString fileName = a.arguments().at(1);
        fileName.replace("\\\\","/");
        fileName.replace("\\","/");
        mw.open(fileName);
    }
    mw.show();

    return a.exec();
}
