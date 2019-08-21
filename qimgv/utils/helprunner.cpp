#include "helprunner.h"

void HelpRunner::run() {
    qDebug().noquote() << qApp->applicationName() << qApp->applicationVersion() << "- A fast and configurable image viewer. Written in qt5.";
    qDebug() << "Website: https://github.com/easymodo/qimgv";
    qDebug() << "License: GNU GPL v3";
    qDebug() << "\nUsage:";
    qDebug() << "   qimgv [options] [file or directory]";
    qDebug() << "\nOptions:";
    qDebug() << "   --help  or  -h    Print this message and exit";

    QStringList features;
#ifdef USE_MPV
    features << "USE_MPV";
#endif
#ifdef USE_KDE_BLUR
    features << "USE_KDE_BLUR";
#endif
    qDebug() << "\nEnabled build options:";
    if(!features.count())
        qDebug() << "   --";
    for(int i = 0; i < features.count(); i++)
        qDebug() << "   " << features.at(i);
    QCoreApplication::exit(0);
}
