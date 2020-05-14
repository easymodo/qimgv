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

void loadStylesheet() {
    // stylesheet
    QFile file(":/res/styles/dark.qss");
    if(file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        // replace color vars -----------------------------
        QColor button("#2f2f30");
        QColor button_hover(button.lighter(115));
        QColor button_pressed(button.darker(120));
        QColor button_border(button.darker(145));

        QColor widget("#232324");

        QColor folderview("#2f2f30");
        QColor folderview_topbar("#1a1a1b");
        QColor folderview_sidebar("#373738");

        QColor accent(settings->accentColor());
        QColor accent_darker(accent.darker(130)); // doesn't work well with colors, needs some desaturation, or even transparency

        QColor slider_groove("#2f2f30");
        QColor slider_handle("#5c5e60");
        QColor slider_handle_hover(slider_handle.lighter(120));

        QColor transparent_overlay("#1a1a1b");  // todo - use for other stuff

        // todo - tweak & reduce to ~3?
        QColor text("#9c9ea0");
        QColor text_light(text.lighter(110));   // previously #c8c8c8
        QColor text_lighter(text.lighter(120)); // #aeaeae
        QColor text_dark(text.darker(110));     // #97999b
        QColor text_darker(text.darker(120));   // #8a8a8a

        // write into stylesheet
        styleSheet.replace("%button%", button.name());
        styleSheet.replace("%button_hover%", button_hover.name());
        styleSheet.replace("%button_pressed%", button_pressed.name());
        styleSheet.replace("%button_border%", button_border.name());
        styleSheet.replace("%widget%", widget.name());
        styleSheet.replace("%folderview%", folderview.name());
        styleSheet.replace("%folderview_topbar%", folderview_topbar.name());
        styleSheet.replace("%folderview_sidebar%", folderview_sidebar.name());
        styleSheet.replace("%accent%", accent.name());
        styleSheet.replace("%accent_darker%", accent_darker.name());
        styleSheet.replace("%slider_groove%", slider_groove.name());
        styleSheet.replace("%slider_handle%", slider_handle.name());
        styleSheet.replace("%slider_handle_hover%", slider_handle_hover.name());
        styleSheet.replace("%transparent_overlay%", transparent_overlay.name());
        styleSheet.replace("%text_lighter%", text_lighter.name());
        styleSheet.replace("%text_light%", text_light.name());
        styleSheet.replace("%text%", text.name());
        styleSheet.replace("%text_dark%", text_dark.name());
        styleSheet.replace("%text_darker%", text_darker.name());

        // ------------------------------------------------
        qApp->setStyleSheet(styleSheet);
    }
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

    loadStylesheet();

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
