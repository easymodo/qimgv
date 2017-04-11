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
    QCoreApplication::setApplicationVersion("0.5dev");


    settings = Settings::getInstance();
    actionManager = ActionManager::getInstance();

    atexit(saveSettings);

    QFile file(":/qss/res/styles/dark.qss");
    if(file.open(QFile::ReadOnly)) {
        QString StyleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(StyleSheet);
    }

    MainWindow mw;
    if(a.arguments().length() > 1) {
        QString fileName = a.arguments().at(1);
        mw.open(fileName);
    }
    mw.show();
    return a.exec();
}
