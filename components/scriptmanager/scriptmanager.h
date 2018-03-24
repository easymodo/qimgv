#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMap>
#include <QDebug>
#include <QStringList>
#include <QProcess>
#include "settings.h"

class Script {
public:
    Script(QString path, QString argumentPattern,bool blocking)
        : mPath(path), mArgumentPattern(argumentPattern), mBlocking(blocking)
    {
    }
    const QString path() { return mPath;   }
    QString argumentPattern() { return mArgumentPattern; }
    bool isBlocking()   { return mBlocking; }

private:
    QString mPath, mArgumentPattern;
    bool mBlocking;
};

class ScriptManager : public QObject {
    Q_OBJECT
public:
    static ScriptManager* getInstance();
    ~ScriptManager();
    void runScript(const QString &scriptName, const QString &arguments);

private:
    explicit ScriptManager(QObject *parent = 0);
    static void initScripts();
    QMap<QString, Script*> scripts; // <name, script>

signals:
    void scriptFinished();
};

extern ScriptManager *scriptManager;

#endif // SCRIPTMANAGER_H
