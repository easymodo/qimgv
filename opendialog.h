#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QFileDialog>
#include <QDebug>
#include "settings.h"

class OpenDialog : QObject
{
    Q_OBJECT
public:
    explicit OpenDialog();
    QString getOpenFileName();

private:
    QFileDialog dialog;
    QWidget *parent;

signals:

public slots:
    void setParent(QWidget*);
};

#endif // OPENDIALOG_H
