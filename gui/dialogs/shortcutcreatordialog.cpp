#include "shortcutcreatordialog.h"
#include "ui_shortcutcreatordialog.h"

ShortcutCreatorDialog::ShortcutCreatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShortcutCreatorDialog),
    shortcut("")
{
    ui->setupUi(this);
    setWindowTitle("Create shortcut");
    actionList = appActions->getList();
    scriptList = scriptManager->scriptNames();

    ui->actionsComboBox->addItems(actionList);
    ui->actionsComboBox->setCurrentIndex(0);

    ui->scriptsComboBox->addItems(scriptList);
    ui->scriptsComboBox->setCurrentIndex(0);
}

ShortcutCreatorDialog::~ShortcutCreatorDialog() {
    delete ui;
}

QString ShortcutCreatorDialog::selectedAction() {
    if(ui->actionsRadioButton->isChecked())
        return ui->actionsComboBox->currentText();
    else
        return "s:"+ui->scriptsComboBox->currentText();
}

QString ShortcutCreatorDialog::selectedShortcut() {
    return ui->sequenceEdit->sequence();
}
