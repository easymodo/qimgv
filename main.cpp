#include <QApplication>
#include "appversion.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"
#include "sharedresources.h"
#include "core.h"

void saveSettings() {
    delete settings;
}

int main(int argc, char *argv[]) {

    // I'm not sure what this does but "1" breaks the UI
    // Huge widgets but tiny fonts
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR","0");

    // for testing purposes
    //qputenv("QT_SCALE_FACTOR","1.0");
    //qputenv("QT_SCREEN_SCALE_FACTORS", "1;1.7");

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("greenpepper software");
    QCoreApplication::setOrganizationDomain("github.com/easymodo/qimgv");
    QCoreApplication::setApplicationName("qimgv");
    QCoreApplication::setApplicationVersion(appVersion.normalized().toString());

    // needed for mpv
    std::setlocale(LC_NUMERIC, "C");

    settings = Settings::getInstance();
    actionManager = ActionManager::getInstance();
    scriptManager = ScriptManager::getInstance();
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
