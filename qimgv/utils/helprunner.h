#pragma once

#include <QApplication>
#include <QObject>
#include <QDebug>
#include <QString>

class HelpRunner : public QObject {
    Q_OBJECT
public slots:
    void run();
};
