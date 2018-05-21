#ifndef SCRIPTEDITORDIALOG_H
#define SCRIPTEDITORDIALOG_H

#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include "components/scriptmanager/scriptmanager.h"
#include "utils/script.h"

namespace Ui {
class ScriptEditorDialog;
}

class ScriptEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptEditorDialog(QWidget *parent = 0);
    explicit ScriptEditorDialog(QString name, Script script, QWidget *parent = 0);
    ~ScriptEditorDialog();
    QString scriptName();
    Script script();

private slots:
    void onNameChanged(QString name);

    void selectScriptPath();
private:
    Ui::ScriptEditorDialog *ui;
};

#endif // SCRIPTEDITORDIALOG_H
