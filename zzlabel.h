#ifndef ZZLABEL_H
#define ZZLABEL_H

#include <QLabel>
#include <QResizeEvent>

class zzLabel : public QLabel
{
    Q_OBJECT
public:
    explicit zzLabel(QWidget *parent = 0);

signals:
    void resized();

public slots:
    void resizeEvent(QResizeEvent *event);

};

#endif // ZZLABEL_H
