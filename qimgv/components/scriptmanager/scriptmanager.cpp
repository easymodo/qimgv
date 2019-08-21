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
        scriptManager->readScripts();
    }
    return scriptManager;
}

#ifdef __linux
void ScriptManager::runScript(const QString &scriptName, std::shared_ptr<Image> img) {
    if(scripts.contains(scriptName)) {
        Script script = scripts.value(scriptName);
        QProcess exec(this);
        QStringList command = splitCommandLine(script.command);
        processArguments(command, img);
        if(script.blocking) {
            exec.start(command.takeAt(0), command);
            if(!exec.waitForStarted())
                qDebug() << "[ScriptManager] Unable not start:" << exec.program() << " Make sure it is an executable.";
            exec.waitForFinished(10000);
        } else {
            if(!exec.startDetached(command.takeAt(0), command)) {
                qWarning() << "[ScriptManager] Unable not start:" << exec.program() << " Make sure it is an executable.";
            }
        }

        emit scriptFinished();
    } else {
        qDebug() << "[ScriptManager] Script " << scriptName << " does not exist.";
    }
}

#else
void ScriptManager::runScript(const QString &scriptName, std::shared_ptr<Image> img) {
    qDebug() << "Nope.";
}
#endif

// TODO: what if filename contains one of the tags?
void ScriptManager::processArguments(QStringList &cmd, std::shared_ptr<Image> img) {
    for (auto& i : cmd) {
        if(i.contains("%file%"))
            i.replace("%file%", img.get()->path());
    }
}

// thanks stackoverflow
QStringList ScriptManager::splitCommandLine(const QString & cmdLine) {
    QStringList list;
    QString arg;
    bool escape = false;
    enum { Idle, Arg, QuotedArg } state = Idle;
    foreach (QChar const c, cmdLine) {
        if (!escape && c == '\\') { escape = true; continue; }
        switch (state) {
        case Idle:
            if (!escape && c == '"') state = QuotedArg;
            else if (escape || !c.isSpace()) { arg += c; state = Arg; }
            break;
        case Arg:
            if (!escape && c == '"') state = QuotedArg;
            else if (escape || !c.isSpace()) arg += c;
            else { list << arg; arg.clear(); state = Idle; }
            break;
        case QuotedArg:
            if (!escape && c == '"') state = arg.isEmpty() ? Idle : Arg;
            else arg += c;
            break;
        }
        escape = false;
    }
    if (!arg.isEmpty()) list << arg;
    return list;
}


bool ScriptManager::scriptExists(QString scriptName) {
    return scripts.contains(scriptName);
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
