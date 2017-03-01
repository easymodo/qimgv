#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QWidget>
#include <QBoxLayout>
#include "clickablelabel.h"
#include <QDebug>

class ToolBox : public QWidget
{
    Q_OBJECT
public:
    explicit ToolBox(QWidget *parent = 0);

private:
    ClickableLabel *rotateLeft, *rotateRight;
    QBoxLayout *buttonsLayout;
signals:
    void rotateLeftClicked();
    void rotateRightClicked();

public slots:
};

#endif // TOOLBOX_H
