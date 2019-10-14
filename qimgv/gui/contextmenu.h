#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QStyleOption>
#include <QTimer>
#include <QPainter>
#include "components/actionmanager/actionmanager.h"
#include "gui/customwidgets/iconbutton.h"

namespace Ui {
class ContextMenu;
}

class ContextMenu : public IconButton {
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
    QStringList reservedKeys;

protected:
    void paintEvent(QPaintEvent *);

    void keyPressEvent(QKeyEvent *event);
};

#endif // CONTEXTMENU_H
