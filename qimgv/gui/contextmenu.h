#pragma once

#include <QStyleOption>
#include <QTimer>
#include <QPainter>
#include "components/actionmanager/actionmanager.h"

namespace Ui {
class ContextMenu;
}

class ContextMenu : public QWidget {
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
    void mousePressEvent(QMouseEvent*);
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *event);
};
