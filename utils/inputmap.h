#ifndef INPUTMAP_H
#define INPUTMAP_H

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
    QMap<quint32, QString> keyMap;
    QMap<QString, Qt::KeyboardModifier> modMap;
};

extern InputMap *inputMap;

#endif // INPUTMAP_H
