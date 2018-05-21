#include "script.h"


Script::Script() : path(""), blocking(false) {
}

Script::Script(QString _path, bool _blocking)
    : path(_path), blocking(_blocking)
{
}
