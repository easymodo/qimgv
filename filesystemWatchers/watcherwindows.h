#ifndef WATCHERWINDOWS_H
#define WATCHERWINDOWS_H

#include <QObject>
#include <QTimer>
#include <QDebug>

class WatcherWindows : public QObject
{
    Q_OBJECT
public:
    explicit WatcherWindows(QObject *parent = 0);
    void setDir(QString _dir);
    QString dir();
    void setUpdateInterval(int msec);

private:
    QTimer mTimer;
    QString mDir;

private slots:
    void loop();

signals:

public slots:
};

#endif // WATCHERWINDOWS_H
