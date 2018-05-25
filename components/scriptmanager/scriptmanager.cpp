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
void ScriptManager::runScript(const QString &scriptName, std::shared_ptr<Image> img) {
    if(scripts.contains(scriptName)) {
        Script script = scripts.value(scriptName);
        QProcess exec(this);

        QStringList command = prepareCommandArguments(script, img);
        exec.setProgram(command.takeAt(0));
        exec.setArguments(command);

        if(script.blocking) {
            exec.start();
            if(!exec.waitForStarted())
                qWarning() << "[ScriptManager] Could not start process.";
            exec.waitForFinished(10000);
        } else {
            exec.startDetached();
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

// TODO: what if filename contains one of the tags?
QStringList ScriptManager::prepareCommandArguments(Script script, std::shared_ptr<Image> img) {
    QStringList arguments = script.path.split(" ");
    for (auto& i : arguments) {
        if(i == "%file%")
            i = img.get()->path();
    }
    return arguments;
}

bool ScriptManager::scriptExists(QString scriptName) {
    return scripts.contains(scriptName);
}

void ScriptManager::initScripts() {
    scriptManager->scripts.insert("TestScript", Script("/home/easymodo/test.sh %file%", false));
    scriptManager->scripts.insert("TestScript2", Script("/home/easymodo/test2.sh %file%", false));
}

void ScriptManager::readScripts() {
    settings->readScripts(scripts);
}

void ScriptManager::saveScripts() {
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
