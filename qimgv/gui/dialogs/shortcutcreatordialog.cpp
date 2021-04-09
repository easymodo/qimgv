#include "shortcutcreatordialog.h"
#include "ui_shortcutcreatordialog.h"

ShortcutCreatorDialog::ShortcutCreatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShortcutCreatorDialog)
{
    ui->setupUi(this);
    setWindowTitle("Add shortcut");
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

void ShortcutCreatorDialog::onShortcutEdited() {
    QString action = actionManager->actionForShortcut(ui->sequenceEdit->sequence());
    if(!action.isEmpty())
        ui->warningLabel->setText("This shortcut is used for action: " + action + ". Replace?");
    else
        ui->warningLabel->setText("");
}

void ShortcutCreatorDialog::setAction(QString action) {
    auto cbox = ui->actionsComboBox;
    if(action.startsWith("s:")) {
        action = action.remove(0,2);
        cbox = ui->scriptsComboBox;
        ui->scriptsRadioButton->setChecked(true);
    }
    int index = cbox->findText(action);
    if(index != -1)
       cbox->setCurrentIndex(index);
}

void ShortcutCreatorDialog::setShortcut(QString shortcut) {
    ui->sequenceEdit->setText(shortcut);
}
