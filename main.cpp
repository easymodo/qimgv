#include <QApplication>
#include "settings.h"
#include "components/actionmanager/actionmanager.h"
#include "core.h"

void saveSettings() {
    delete settings;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("greenpepper software");
    QCoreApplication::setOrganizationDomain("github.com/easymodo/qimgv");
    QCoreApplication::setApplicationName("qimgv");
    QCoreApplication::setApplicationVersion("0.50dev");

    // needed for mpv
    std::setlocale(LC_NUMERIC, "C");

    settings = Settings::getInstance();
    actionManager = ActionManager::getInstance();

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
