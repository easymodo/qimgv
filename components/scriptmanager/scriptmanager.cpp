#include "scriptmanager.h"

ScriptManager *scriptManager = nullptr;

ScriptManager::ScriptManager(QObject *parent)
    : QObject(parent)
{
}

ScriptManager::~ScriptManager() {
    delete scriptManager;
}

ScriptManager *ScriptManager::getInstance() {
    if(!scriptManager) {
        scriptManager = new ScriptManager();
        initScripts();
    }
    return scriptManager;
}

#ifdef __linux
void ScriptManager::runScript(const QString &scriptName, const QString &arguments) {
    qDebug() << arguments;
    if(scripts.contains(scriptName)) {
        QProcess exec(this);
        Script *script = scripts.value(scriptName);
        if(script->isBlocking()) {
            qDebug() << "Starting blocking script.";
            exec.start("/bin/sh", QStringList() << script->path() << arguments);
            qDebug() << "Waiting for script to finish...";
            exec.waitForFinished(-1);
            qDebug() << "Script finished.";
        } else {
            qDebug() << "Starting detached script.";
            //exec.startDetached("/bin/sh", QStringList() << script->path() << arguments);
            exec.startDetached("/bin/sh", QStringList() << script->path() << arguments);
        }
        emit scriptFinished();
    } else {
        qDebug() << "Script " << scriptName << " does not exist.";
    }
}
#else
void ScriptManager::runScript(const QString &scriptName, const QString &arguments) {
    qDebug() << "Nope.";
}
#endif

void ScriptManager::initScripts() {
    scriptManager->scripts.insert("TestScript", new Script("/home/easymodo/test.sh", "%filepath%", false));
    scriptManager->scripts.insert("TestScript", new Script("/home/easymodo/test.sh", "%dir%", false));
}
