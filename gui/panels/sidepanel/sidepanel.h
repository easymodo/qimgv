#ifndef SIDEPANEL_H
#define SIDEPANEL_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>

namespace Ui {
class SidePanel;
}

class SidePanel : public QWidget
{
    Q_OBJECT

public:
    explicit SidePanel(QWidget *parent = 0);
    ~SidePanel();

    void setWidget(QWidget *w);
    QWidget* widget();

public slots:
    void show();
    void hide();

protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::SidePanel *ui;
    QWidget *mWidget;
};

#endif // SIDEPANEL_H
