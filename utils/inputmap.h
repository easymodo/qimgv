#ifndef INPUTMAP_H
#define INPUTMAP_H

#include <QMap>
#include <QString>

class InputMap {
public:
    InputMap();
    static InputMap *getInstance();
    const QMap<int, QString> &keys();
    const QMap<QString, int> &keysReverse();
    const QMap<QString, Qt::KeyboardModifier> &modifiers();

private:
    void initKeyMap();
    void initModMap();
    QMap<int, QString> keyMap;
    QMap<QString, int> keyMapReverse;
    QMap<QString, Qt::KeyboardModifier> modMap;
};

extern InputMap *inputMap;

#endif // INPUTMAP_H
