#pragma once

#include <QMap>
#include <QString>
#include <QVersionNumber>

class Actions
{
public:
    Actions();
    static Actions *getInstance();
    const QMap<QString, QVersionNumber> &getMap();
    QList<QString> getList();

private:
    void init();
    QMap<QString, QVersionNumber> mActions;

};

extern Actions *appActions;
