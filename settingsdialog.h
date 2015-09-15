#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QColorDialog>
#include <QDebug>
#include "clickablelabel.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private:
    QPalette bgLabelPalette;
    void readSettings();
    Ui::SettingsDialog *ui;
    static constexpr int thumbSizeSmall = 100,
              thumbSizeMedium = 120,
              thumbSizeLarge = 140,
              thumbSizeVeryLarge = 160;

    int thumbSizeCustom;

private slots:
    void applySettings();
    void applySettingsAndClose();
    void bgColorDialog();

signals:
    void settingsChanged();
};

#endif // SETTINGSDIALOG_H
