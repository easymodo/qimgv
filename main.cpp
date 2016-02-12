#include "core.h"
#include "mainwindow.h"
#include <QApplication>
#include "settings.h"
#include "actionmanager.h"

void saveSettings() {
    delete settings;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("greenpepper software");
    QCoreApplication::setOrganizationDomain("github.com/easymodo/qimgv");
    QCoreApplication::setApplicationName("qimgv");
    QCoreApplication::setApplicationVersion("0.45.1");

    settings = Settings::getInstance();
    actionManager = ActionManager::getInstance();

    atexit(saveSettings);

    QFile file(":/styles/res/style.qss");
    if(file.open(QFile::ReadOnly)) {
        QString StyleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(StyleSheet);
    }

    MainWindow mw;
    if(settings->fullscreenMode()) {
        mw.slotTriggerFullscreen();
    }
    if(a.arguments().length() > 1) {
        QString fileName = a.arguments().at(1);
        mw.open(fileName);
    }
    mw.show();
    return a.exec();
}
