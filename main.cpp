#include <QApplication>
#include "settings.h"
#include "components/actionmanager/actionmanager.h"
#include "sharedresources.h"
#include "core.h"

void saveSettings() {
    delete settings;
}

int main(int argc, char *argv[]) {
    //qputenv("QT_AUTO_SCREEN_SCALE_FACTOR","0");
    //qputenv("QT_SCREEN_SCALE_FACTORS", "1;1.4");
    //qputenv("QT_SCALE_FACTOR","2");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("greenpepper software");
    QCoreApplication::setOrganizationDomain("github.com/easymodo/qimgv");
    QCoreApplication::setApplicationName("qimgv");
    QCoreApplication::setApplicationVersion("0.6");

    // needed for mpv
    std::setlocale(LC_NUMERIC, "C");

    settings = Settings::getInstance();
    actionManager = ActionManager::getInstance();
    shrRes = SharedResources::getInstance();

    atexit(saveSettings);

    QFile file(":/res/styles/dark.qss");
    if(file.open(QFile::ReadOnly)) {
        QString StyleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(StyleSheet);
    }

    Core core;
    if(a.arguments().length() > 1) {
        QString fileName = a.arguments().at(1);
        core.loadByPathBlocking(fileName);
    }
    core.showGui();

    return a.exec();
}
