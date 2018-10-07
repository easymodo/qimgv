#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include "gui/customwidgets/clickablewidget.h"
#include <QStyleOption>
#include <QTimer>
#include <QPainter>
#include "components/actionmanager/actionmanager.h"

namespace Ui {
class ContextMenu;
}

class ContextMenu : public ClickableWidget {
    Q_OBJECT
public:
    explicit ContextMenu(QWidget *parent = nullptr);
    ~ContextMenu();
    void setImageEntriesEnabled(bool mode);

public slots:
    void showAt(QPoint pos);
private:
    Ui::ContextMenu *ui;
    QTimer mTimer;

protected:
    void paintEvent(QPaintEvent *);

};

#endif // CONTEXTMENU_H
