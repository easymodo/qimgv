#include "scriptmanager.h"

ScriptManager *scriptManager = nullptr;

ScriptManager::ScriptManager(QObject *parent)
    : QObject(parent)
{
}

ScriptManager::~ScriptManager() {
    scriptManager->saveScripts();
    delete scriptManager;
}

ScriptManager *ScriptManager::getInstance() {
    if(!scriptManager) {
        scriptManager = new ScriptManager();
        //scriptManager->initScripts(); // tmp
        scriptManager->readScripts();
    }
    return scriptManager;
}

#ifdef __linux
void ScriptManager::runScript(const QString &scriptName, const QString &arguments) {
    qDebug() << "runScript()" << scriptName << arguments;
    if(scripts.contains(scriptName)) {
        QProcess exec(this);
        Script script = scripts.value(scriptName);
        if(script.blocking) {
            qDebug() << "[ScriptManager] Starting blocking script.";
            exec.start("/bin/sh", QStringList() << script.path << arguments);
            exec.waitForFinished(10000);
            qDebug() << "[ScriptManager] Script finished.";
        } else {
            qDebug() << "[ScriptManager] Starting detached script.";
            exec.startDetached("/bin/sh", QStringList() << script.path << arguments);
        }
        emit scriptFinished();
    } else {
        qDebug() << "[ScriptManager] Script " << scriptName << " does not exist.";
    }
}

#else
void ScriptManager::runScript(const QString &scriptName, const QString &arguments) {
    qDebug() << "Nope.";
}
#endif

bool ScriptManager::scriptExists(QString scriptName) {
    return scripts.contains(scriptName);
}

void ScriptManager::initScripts() {
    scriptManager->scripts.insert("TestScript", Script("/home/easymodo/test.sh", false));
    scriptManager->scripts.insert("TestScript2", Script("/home/easymodo/test2.sh", false));
}

void ScriptManager::readScripts() {
    settings->readScripts(scripts);
    qDebug() << "read: " << scripts.count();
}

void ScriptManager::saveScripts() {
    qDebug() << "save: " << scripts.count();
    settings->saveScripts(scripts);
}

// replaces if it already exists
void ScriptManager::addScript(QString scriptName, Script script) {
    if(scripts.contains(scriptName)) {
        qDebug() << "[ScriptManager] Replacing script" << scriptName;
        scripts.remove(scriptName);
    }
    scripts.insert(scriptName, script);
}

void ScriptManager::removeScript(QString scriptName) {
    scripts.remove(scriptName);
}

const QMap<QString, Script> &ScriptManager::allScripts() {
    return scriptManager->scripts;
}

QList<QString> ScriptManager::scriptNames() {
    return scriptManager->scripts.keys();
}

Script ScriptManager::getScript(QString scriptName) {
    return scripts.value(scriptName);
}
