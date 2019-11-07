#pragma once

#include <QString>

#ifdef WIN32
    #define StdString std::wstring
#else
    #define StdString std::string
#endif

int clamp(int x, int lower, int upper);
int probeOS();
StdString toStdString(QString str);
QString fromStdString(StdString str);
