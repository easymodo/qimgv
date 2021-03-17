#pragma once

#include <QDialog>
#include <QComboBox>
#include <QRadioButton>
#include "shortcutbuilder.h"
#include "utils/actions.h"
#include "components/actionmanager/actionmanager.h"
#include "components/scriptmanager/scriptmanager.h"
// TODO: separate gui from components
// OR move script & action stuff to project root?

namespace Ui {
class ShortcutCreatorDialog;
}

class ShortcutCreatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortcutCreatorDialog(QWidget *parent = nullptr);
    ~ShortcutCreatorDialog();
    QString selectedAction();
    QString selectedShortcut();
    void setAction(QString);
    void setShortcut(QString);

private slots:
    void onShortcutEdited();

private:
    Ui::ShortcutCreatorDialog *ui;
    QList<QString> actionList, scriptList;
};
