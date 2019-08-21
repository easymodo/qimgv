#ifndef HELPRUNNER_H
#define HELPRUNNER_H

#include <QApplication>
#include <QObject>
#include <QDebug>
#include <QString>

class HelpRunner : public QObject {
    Q_OBJECT
public slots:
    void run();
};

#endif // HELPRUNNER_H
