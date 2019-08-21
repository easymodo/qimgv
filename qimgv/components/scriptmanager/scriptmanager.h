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
#include "utils/script.h"
#include "sourcecontainers/image.h"
#include "settings.h"

class ScriptManager : public QObject {
    Q_OBJECT
public:
    static ScriptManager* getInstance();
    ~ScriptManager();
    void runScript(const QString &scriptName, std::shared_ptr<Image> img);
    bool scriptExists(QString scriptName);
    void readScripts();
    void saveScripts();
    void removeScript(QString scriptName);
    const QMap<QString, Script> &allScripts();
    QList<QString> scriptNames();
    Script getScript(QString scriptName);

    void addScript(QString scriptName, Script script);
private:
    explicit ScriptManager(QObject *parent = nullptr);
    QMap<QString, Script> scripts; // <name, script>
    void processArguments(QStringList &cmd, std::shared_ptr<Image> img);

    QStringList splitCommandLine(const QString &cmdLine);
signals:
    void scriptFinished();
private slots:
};

extern ScriptManager *scriptManager;

#endif // SCRIPTMANAGER_H
