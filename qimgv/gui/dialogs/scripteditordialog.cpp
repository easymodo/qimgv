#include "scripteditordialog.h"
#include "ui_scripteditordialog.h"

ScriptEditorDialog::ScriptEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptEditorDialog),
    editMode(false)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("New application/script"));
    ui->keywordsLabel->setText(tr("Keywords:") + " %file%");
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this, &ScriptEditorDialog::onNameChanged);
    this->onNameChanged(ui->nameLineEdit->text());
}

ScriptEditorDialog::ScriptEditorDialog(QString name, Script script, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ScriptEditorDialog),
      editMode(true)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Edit"));
    this->onNameChanged(ui->nameLineEdit->text());
    editTarget = name;
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this, &ScriptEditorDialog::onNameChanged);
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
        ui->messageLabel->setText(tr("Enter script name"));
        ui->acceptButton->setEnabled(false);
        return;
    } else {
        ui->acceptButton->setEnabled(true);
    }

    QString okBtnText;
    ui->messageLabel->clear();

    if(editMode) {
        if(name != editTarget && scriptManager->scriptExists(name)) {
            ui->messageLabel->setText(tr("A script with this same name exists"));
            okBtnText = "Replace";
        } else {
            okBtnText = "Save";
        }
    } else {
        if(scriptManager->scriptExists(name)) {
            ui->messageLabel->setText(tr("A script with this same name exists"));
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
#ifdef _WIN32
    file = dialog.getOpenFileName(this, tr("Select an executable/script"), "", "Executable/script (*.exe *.bat)");
#else
    file = dialog.getOpenFileName(this, tr("Select a script file"), "", "Shell script (*.sh)");
#endif
    if(!file.isEmpty()) {
        ui->pathLineEdit->setText("\"" + file + "\"" + " %file%");
    }
}
