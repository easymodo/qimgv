#include "scripteditordialog.h"
#include "ui_scripteditordialog.h"

ScriptEditorDialog::ScriptEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptEditorDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("New script");
    connect(ui->nameLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(onNameChanged(QString)));
    this->onNameChanged(ui->nameLineEdit->text());
}

ScriptEditorDialog::ScriptEditorDialog(QString name, Script script, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ScriptEditorDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Edit script");
    this->onNameChanged(ui->nameLineEdit->text());
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
    } else if(scriptManager->scriptExists(name)) {
        ui->messageLabel->setText("Script with the same name already exists.");
        ui->acceptButton->setText("Replace");
        ui->acceptButton->setEnabled(true);
    } else {
        ui->messageLabel->clear();
        ui->acceptButton->setText("Create");
        ui->acceptButton->setEnabled(true);
    }
}

void ScriptEditorDialog::selectScriptPath() {
    QFileDialog dialog;
    QString file;
    file = dialog.getOpenFileName(this, "Select a shell script", "", "Shell script (*.sh)");
    if(!file.isEmpty()) {
        ui->pathLineEdit->setText(file + " %file%");
    }
}
