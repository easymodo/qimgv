#ifndef SIDEPANELWIDGET_H
#define SIDEPANELWIDGET_H

#include <QObject>
#include <QWidget>

class SidePanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SidePanelWidget(QWidget *parent = nullptr);

signals:

public slots:
    virtual void show() = 0;
};

#endif // SIDEPANELWIDGET_H
