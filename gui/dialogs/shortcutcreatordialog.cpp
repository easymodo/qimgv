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
    ui->actionsComboBox->addItems(actionList);
    ui->actionsComboBox->setCurrentIndex(0);
}

ShortcutCreatorDialog::~ShortcutCreatorDialog() {
    delete ui;
}

QString ShortcutCreatorDialog::selectedAction() {
    return ui->actionsComboBox->currentText();
}

QString ShortcutCreatorDialog::selectedShortcut() {
    return ui->sequenceEdit->sequence();
}
