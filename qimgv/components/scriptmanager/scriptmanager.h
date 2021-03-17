#pragma once

#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMap>
#include <QDebug>
#include <QStringList>
#include <QProcess>
#include "utils/script.h"
#include "sourcecontainers/image.h"
#include "settings.h"

class ScriptManager : public QObject {
    Q_OBJECT
public:
    static ScriptManager* getInstance();
    ~ScriptManager();
    void runScript(const QString &scriptName, std::shared_ptr<Image> img);
    static QString runCommand(QString cmd);
    static void runCommandDetached(QString cmd);
    bool scriptExists(QString scriptName);
    void readScripts();
    void saveScripts();
    void removeScript(QString scriptName);
    const QMap<QString, Script> &allScripts();
    QList<QString> scriptNames();
    Script getScript(QString scriptName);
    void addScript(QString scriptName, Script script);
    static QStringList splitCommandLine(const QString &cmdLine);

signals:
    void error(QString);

private:
    explicit ScriptManager(QObject *parent = nullptr);
    QMap<QString, Script> scripts; // <name, script>
    void processArguments(QStringList &cmd, std::shared_ptr<Image> img);

};

extern ScriptManager *scriptManager;
