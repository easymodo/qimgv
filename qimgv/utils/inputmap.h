#pragma once

#include <QMap>
#include <QString>

class InputMap {
public:
    InputMap();
    static InputMap *getInstance();
    const QMap<quint32, QString> &keys();
    const QMap<QString, Qt::KeyboardModifier> &modifiers();

private:
    void initKeyMap();
    void initModMap();
    QMultiMap<quint32, QString> keyMap;
    QMap<QString, Qt::KeyboardModifier> modMap;
};

extern InputMap *inputMap;
