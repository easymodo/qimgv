#include "scripteditordialog.h"
#include "ui_scripteditordialog.h"

ScriptEditorDialog::ScriptEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptEditorDialog),
    editMode(false)
{
    ui->setupUi(this);
    this->setWindowTitle("New script");
    connect(ui->nameLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(onNameChanged(QString)));
    this->onNameChanged(ui->nameLineEdit->text());
}

ScriptEditorDialog::ScriptEditorDialog(QString name, Script script, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ScriptEditorDialog),
      editMode(true)
{
    ui->setupUi(this);
    this->setWindowTitle("Edit script");
    this->onNameChanged(ui->nameLineEdit->text());
    editTarget = name;
    connect(ui->nameLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(onNameChanged(QString)));
    ui->nameLineEdit->setText(name);
    ui->pathLineEdit->setText(script.command);
    ui->blockingCheckBox->setChecked(script.blocking);
    this->onNameChanged(ui->nameLineEdit->text());
}

ScriptEditorDialog::~ScriptEditorDialog() {
    delete ui;
}

QString ScriptEditorDialog::scriptName() {
    return ui->nameLineEdit->text();
}

Script ScriptEditorDialog::script() {
    return Script(ui->pathLineEdit->text(), ui->blockingCheckBox->isChecked());
}

void ScriptEditorDialog::onNameChanged(QString name) {
    if(name.isEmpty()) {
        ui->messageLabel->setText("Enter script name");
        ui->acceptButton->setEnabled(false);
        return;
    } else {
        ui->acceptButton->setEnabled(true);
    }

    QString okBtnText;
    ui->messageLabel->clear();

    if(editMode) {
        if(name != editTarget && scriptManager->scriptExists(name)) {
            ui->messageLabel->setText("A script with this same name exists.");
            okBtnText = "Replace";
        } else {
            okBtnText = "Save";
        }
    } else {
        if(scriptManager->scriptExists(name)) {
            ui->messageLabel->setText("A script with this same name exists.");
            okBtnText = "Replace";
        } else {
            okBtnText = "Create";
        }
    }
    ui->acceptButton->setText(okBtnText);
}

void ScriptEditorDialog::selectScriptPath() {
    QFileDialog dialog;
    QString file;
    file = dialog.getOpenFileName(this, "Select a shell script", "", "Shell script (*.sh)");
    if(!file.isEmpty()) {
        ui->pathLineEdit->setText(file + " %file%");
    }
}
