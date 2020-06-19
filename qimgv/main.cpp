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
    QFile file(":/res/styles/style-template.qss");
    if(file.open(QFile::ReadOnly)) {
        auto theme = settings->theme();
        QString styleSheet = QLatin1String(file.readAll());

        // -------------- write variables into stylesheet ---------------
        styleSheet.replace("%icontheme%",            settings->theme().iconTheme);
        styleSheet.replace("%button%",               theme.colors.button.name());
        styleSheet.replace("%button_hover%",         theme.colors.button_hover.name());
        styleSheet.replace("%button_pressed%",       theme.colors.button_pressed.name());
        styleSheet.replace("%panel_button_hover%",   theme.colors.panel_button_hover.name());
        styleSheet.replace("%panel_button_pressed%", theme.colors.panel_button_pressed.name());
        styleSheet.replace("%button_border%",        theme.colors.button_border.name());
        styleSheet.replace("%widget%",               theme.colors.widget.name());
        styleSheet.replace("%widget_border%",        theme.colors.widget_border.name());
        styleSheet.replace("%folderview%",           theme.colors.folderview.name());
        styleSheet.replace("%folderview_topbar%",    theme.colors.folderview_topbar.name());
        styleSheet.replace("%folderview_separator%", theme.colors.folderview_separator.name());
        styleSheet.replace("%accent%",               theme.colors.accent.name());
        styleSheet.replace("%accent_lc%",            theme.colors.accent_lc.name());
        styleSheet.replace("%input_field_focus%",    theme.colors.input_field_focus.name());
        styleSheet.replace("%slider_groove%",        theme.colors.slider_groove.name());
        styleSheet.replace("%slider_handle%",        theme.colors.slider_handle.name());
        styleSheet.replace("%slider_hover%",         theme.colors.slider_hover.name());
        styleSheet.replace("%overlay%",              theme.colors.overlay.name());
        styleSheet.replace("%text_hc2%",             theme.colors.text_hc2.name());
        styleSheet.replace("%text_hc1%",             theme.colors.text_hc1.name());
        styleSheet.replace("%text%",                 theme.colors.text.name());
        styleSheet.replace("%overlay_text%",         theme.colors.overlay_text.name());
        styleSheet.replace("%text_lc1%",             theme.colors.text_lc1.name());
        styleSheet.replace("%text_lc2%",             theme.colors.text_lc2.name());
        styleSheet.replace("%system_window_tinted%", theme.system_window_tinted.name());

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
