#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <QFrame>

namespace Ui {
class ScriptWidget;
}

class ScriptWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ScriptWidget(QWidget *parent = 0);
    ~ScriptWidget();

private:
    Ui::ScriptWidget *ui;
};

#endif // SCRIPTWIDGET_H
