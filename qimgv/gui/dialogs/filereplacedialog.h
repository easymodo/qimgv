#pragma once
#include <QDialog>

struct DialogResult {
    bool yes = false;
    bool all = false;
    bool cancel = false;
    /*DIALOG_YES,
    DIALOG_YESTOALL,
    DIALOG_NO,
    DIALOG_NOTOALL,
    DIALOG_CANCEL*/
    bool operator==(bool const &cmp) const {
        return yes == cmp;
    }
    operator bool() {
        return yes;
    }
};

enum FileReplaceMode {
    FILE_TO_FILE,
    DIR_TO_DIR,
    FILE_TO_DIR,
    DIR_TO_FILE
};

namespace Ui {
class FileReplaceDialog;
}

class FileReplaceDialog : public QDialog {
    Q_OBJECT

public:
    explicit FileReplaceDialog(QWidget *parent = nullptr);
    ~FileReplaceDialog();

    void setMode(FileReplaceMode mode);
    void setMulti(bool);
    DialogResult getResult();

    void setSource(QString src);
    void setDestination(QString dst);
private slots:
    void onYesClicked();
    void onNoClicked();
    void onCancelClicked();

private:
    Ui::FileReplaceDialog *ui;
    bool multi;
    DialogResult result;
};
