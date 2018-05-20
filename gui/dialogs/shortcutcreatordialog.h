#ifndef SHORTCUTCREATORDIALOG_H
#define SHORTCUTCREATORDIALOG_H

#include <QDialog>
#include <QComboBox>
#include "shortcutbuilder.h"
#include "utils/actions.h"

namespace Ui {
class ShortcutCreatorDialog;
}

class ShortcutCreatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortcutCreatorDialog(QWidget *parent = 0);
    ~ShortcutCreatorDialog();
    QString selectedAction();
    QString selectedShortcut();

private:
    Ui::ShortcutCreatorDialog *ui;
    int selectedActionIndex;
    QString shortcut;
    QList<QString> actionList;
};

#endif // SHORTCUTCREATORDIALOG_H
