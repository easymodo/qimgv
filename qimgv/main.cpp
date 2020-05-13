#include <QApplication>
#include "appversion.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"
#include "utils/inputmap.h"
#include "utils/actions.h"
#include "utils/helprunner.h"
#include "sharedresources.h"
#include "proxystyle.h"
#include "core.h"

void saveSettings() {
    delete settings;
}

//------------------------------------------------------------------------------
QDataStream& operator<<(QDataStream& out, const Script& v) {
    out << v.command << v.blocking;
    return out;
}
//------------------------------------------------------------------------------
QDataStream& operator>>(QDataStream& in, Script& v) {
    in >> v.command;
    in >> v.blocking;
    return in;
}

int main(int argc, char *argv[]) {
    // I'm not sure what this does but "1" breaks the UI
    // Huge widgets but tiny fonts
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR","0");

    // for testing purposes
    //qputenv("QT_SCALE_FACTOR","1.4");
    //qputenv("QT_SCREEN_SCALE_FACTORS", "1;1.7");

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("qimgv");
    QCoreApplication::setOrganizationDomain("github.com/easymodo/qimgv");
    QCoreApplication::setApplicationName("qimgv");
    QCoreApplication::setApplicationVersion(appVersion.normalized().toString());

    a.setStyle(new ProxyStyle);

    QGuiApplication::setDesktopFileName(QCoreApplication::applicationName() + ".desktop");

    // enable translations
    QString localeName = QLocale::system().name();
    //QTranslator translator;
    //translator.load(":/res/translations/" + localeName);
    //a.installTranslator(&translator);
    //qDebug() << "localeName: " << localeName;

    // get arguments
    QString arg1;
    if(a.arguments().length() > 1) {
        arg1 = a.arguments().at(1);
    }

    // output help text & shutdown
    if(arg1 == "-h" || arg1 == "--help") {
        HelpRunner r;
        QTimer::singleShot(0, &r, SLOT(run()));
        return a.exec();
    }

    // init
    // needed for mpv
#ifndef _MSC_VER
    setlocale(LC_NUMERIC, "C");
#endif
    qRegisterMetaTypeStreamOperators<Script>("Script");

    inputMap = InputMap::getInstance();
    appActions = Actions::getInstance();
    settings = Settings::getInstance();
    scriptManager = ScriptManager::getInstance();
    actionManager = ActionManager::getInstance();
    shrRes = SharedResources::getInstance();

    atexit(saveSettings);

    QElapsedTimer t;
    t.start();

    // stylesheet
    QFile file(":/res/styles/dark.qss");
    if(file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        // replace color vars -----------------------------

        // text - 0.9.1
        /*
        styleSheet.replace("%text0%", "#c8c8c8");
        styleSheet.replace("%text1%", "#aeaeae");
        styleSheet.replace("%text2%", "#9c9ea0");
        styleSheet.replace("%text3%", "#97999b");
        styleSheet.replace("%text4%", "#8a8a8a");
        */
        styleSheet.replace("%text0%", "black");
        styleSheet.replace("%text1%", "black");
        styleSheet.replace("%text2%", "black");
        styleSheet.replace("%text3%", "black");
        styleSheet.replace("%text4%", "black");

        // borders - 0.9.1
        //styleSheet.replace("%border2%", "#1d1d1e");
        //styleSheet.replace("%border3%", "#404042"); // lineedit

        styleSheet.replace("%border2%", "black");
        styleSheet.replace("%border3%", "green"); // lineedit


        // surfaces - 0.9.1
        /*
        styleSheet.replace("%bg0%", "#2f2f30");
        styleSheet.replace("%bg1%", "#232324");
        styleSheet.replace("%bg2%", "#1a1a1b");
        styleSheet.replace("%bg3%", "#373738");
        styleSheet.replace("%bg4%", "#2a2a2b"); // button-pressed
        styleSheet.replace("%bg5%", "#3e3e3e");
        styleSheet.replace("%bg6%", "#464646"); // placesPanel-hover
        styleSheet.replace("%bg7%", "#545454"); // placesPanel-selected
        styleSheet.replace("%bg8%", "#5c5e60"); // scrollbar
        styleSheet.replace("%bg9%", "#6f7274"); // scrollbar-hover
        */

        styleSheet.replace("%bg0%", "#8093e9");
        styleSheet.replace("%bg1%", "#6e7ec8");
        styleSheet.replace("%bg2%", "#485384");
        styleSheet.replace("%bg3%", "#8a74d3"); // hover
        styleSheet.replace("%bg4%", "#635398"); // button-pressed
        styleSheet.replace("%bg5%", "#866398");
        styleSheet.replace("%bg6%", "#986086"); // placesPanel-hover
        styleSheet.replace("%bg7%", "#984585"); // placesPanel-selected
        styleSheet.replace("%bg8%", "#b14b69"); // scrollbar/slider
        styleSheet.replace("%bg9%", "#d95c81"); // scrollbar/slider-hover

        // ------------------------------------------------
        qApp->setStyleSheet(styleSheet);
    }

    //qApp->processEvents();
    //qDebug() << "stylesheet: " << t.elapsed() << "ms";

    Core core;

    //qApp->processEvents();
    //qDebug() << "Core + MW init: " << t.elapsed() << "ms";

    if(!arg1.isEmpty()) {
        // assume 1st arg is the filename
        core.loadPath(arg1);
    } else if(settings->defaultViewMode() == MODE_FOLDERVIEW) {
        core.loadPath(QDir::homePath());
    }
    // wait for event queue to catch up before showing window
    // this avoids white background flicker
    qApp->processEvents();
    core.showGui();

    //qApp->processEvents();
    //qDebug() << "showGui: " << t.elapsed() << "ms";

    return a.exec();
}
