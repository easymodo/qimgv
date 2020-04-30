#pragma once

#include <QString>

#ifdef _WIN32
    #define StdString std::wstring
    #define CharType wchar_t
#else
    #define StdString std::string
    #define CharType char
#endif

int clamp(int x, int lower, int upper);
int probeOS();
StdString toStdString(QString str);
QString fromStdString(StdString str);
