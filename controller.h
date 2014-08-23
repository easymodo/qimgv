#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "directorymanager.h"
#include "imageloader.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);

signals:

public slots:

};

#endif // CONTROLLER_H
