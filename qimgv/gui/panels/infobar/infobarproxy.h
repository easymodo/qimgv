#ifndef INFOBARPROXY_H
#define INFOBARPROXY_H

#include "gui/panels/infobar/infobar.h"
#include <QVBoxLayout>

struct InfoBarStateBuffer {
    QString position;
    QString fileName;
    QString info;
};

class InfoBarProxy : public QWidget {
    Q_OBJECT
public:
    explicit InfoBarProxy(QWidget *parent = nullptr);
    ~InfoBarProxy();

    void init();
public slots:
    void setInfo(QString position, QString fileName, QString info);

private:
    InfoBarStateBuffer stateBuf;
    InfoBar *infoBar;
    QVBoxLayout layout;
};

#endif // INFOBARPROXY_H
