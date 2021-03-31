#include "filereplacedialog.h"
#include "ui_filereplacedialog.h"

FileReplaceDialog::FileReplaceDialog(QWidget *parent) : QDialog(parent), ui(new Ui::FileReplaceDialog) {
    ui->setupUi(this);
    multi = false;
    result = DIALOG_CANCEL;
    connect(ui->yesButton, &QPushButton::clicked, this, &FileReplaceDialog::onYesClicked);
    connect(ui->noButton, &QPushButton::clicked, this, &FileReplaceDialog::onNoClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &FileReplaceDialog::onCancelClicked);
}

FileReplaceDialog::~FileReplaceDialog() {
    delete ui;
}

void FileReplaceDialog::setSource(QString src) {
    ui->srcLabel->setText(src);
}

void FileReplaceDialog::setDestination(QString dst) {
    ui->dstLabel->setText(dst);
}

void FileReplaceDialog::setMode(FileReplaceMode mode) {
    if(mode == FILE_TO_FILE) {
        setWindowTitle("File already exists");
        ui->titleLabel->setText("Replace destination file?");
    } else if(mode == DIR_TO_DIR) {
        setWindowTitle("Directory already exists");
        ui->titleLabel->setText("Merge directories?");
    } else if(mode == DIR_TO_FILE) {
        setWindowTitle("Destination already exists");
        ui->titleLabel->setText("There is a file with that name. Replace?");
    } else { // FILE_TO_DIR
        setWindowTitle("Destination already exists");
        ui->titleLabel->setText("There is a folder with that name. Replace?");
    }
}

void FileReplaceDialog::setMulti(bool _multi) {
    multi = _multi;
    ui->applyAllCheckBox->setVisible(multi);
}

DialogResult FileReplaceDialog::getResult() {
    return result;
}

void FileReplaceDialog::onYesClicked() {
    if(multi && ui->applyAllCheckBox->isChecked())
        result = DIALOG_YESTOALL;
    else
        result = DIALOG_YES;
    this->close();
}

void FileReplaceDialog::onNoClicked() {
    //if(multi && ui->applyAllCheckBox->isChecked())
    //    result = DIALOG_NOTOALL;
    //else
        result = DIALOG_NO;
    this->close();
}

void FileReplaceDialog::onCancelClicked() {
    this->close();
}
