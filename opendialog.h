#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QFileDialog>
#include <QDebug>

class OpenDialog : QObject
{
    Q_OBJECT
public:
    explicit OpenDialog();
    QString getOpenFileName();

private:
    QFileDialog dialog;
    QString dir;
    QWidget *parent;

signals:

public slots:
    void setDirectory(QString);
    void setParent(QWidget*);
};

#endif // OPENDIALOG_H
