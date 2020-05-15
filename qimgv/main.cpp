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
    // stylesheet template file
    QFile file(":/res/styles/dark.qss");
    if(file.open(QFile::ReadOnly)) {
        auto colors = settings->colorScheme();
        QString styleSheet = QLatin1String(file.readAll());
        // -------------- write colors into stylesheet ---------------
        styleSheet.replace("%button%",            colors.button.name());
        styleSheet.replace("%button_hover%",      colors.button_hover.name());
        styleSheet.replace("%button_pressed%",    colors.button_pressed.name());
        styleSheet.replace("%button_border%",     colors.button_border.name());
        styleSheet.replace("%widget%",            colors.widget.name());
        styleSheet.replace("%widget_border%",     colors.widget_border.name());
        styleSheet.replace("%folderview%",        colors.folderview.name());
        styleSheet.replace("%folderview_topbar%", colors.folderview_topbar.name());
        styleSheet.replace("%folderview_panel%",  colors.folderview_panel.name());
        styleSheet.replace("%accent%",            colors.accent.name());
        styleSheet.replace("%accent_darker%",     colors.accent_darker.name());
        styleSheet.replace("%input_field_focus%", colors.input_field_focus.name());
        styleSheet.replace("%slider_groove%",     colors.slider_groove.name());
        styleSheet.replace("%slider_handle%",     colors.slider_handle.name());
        styleSheet.replace("%slider_hover%",      colors.slider_hover.name());
        styleSheet.replace("%overlay%",           colors.overlay.name());
        styleSheet.replace("%text_lighter%",      colors.text_lighter.name());
        styleSheet.replace("%text_light%",        colors.text_light.name());
        styleSheet.replace("%text%",              colors.text.name());
        styleSheet.replace("%overlay_text%",      colors.overlay_text.name());
        styleSheet.replace("%text_dark%",         colors.text_dark.name());
        styleSheet.replace("%text_darker%",       colors.text_darker.name());
        // ------------------------ apply ----------------------------
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
