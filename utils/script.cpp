#include "script.h"


Script::Script() : command(""), blocking(false) {
}

Script::Script(QString _path, bool _blocking)
    : command(_path), blocking(_blocking)
{
}
