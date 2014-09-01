#include "core.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "image.h"

//#include <iostream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Core *c = new Core();
    c->setCurrentDir("K:\\_code\\sample images");

    MainWindow *mw = new MainWindow();
    c->connectGui(mw);
    mw->show();

    if(a.arguments().length()>1) {
        //mw->slotTriggerFullscreen();
        c->open(a.arguments().at(1));
    }
    return a.exec();
}
