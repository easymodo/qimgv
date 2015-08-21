#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>
#include <QColorDialog>
#include <QDebug>
#include "clickablelabel.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private:
    QPalette bgLabelPalette;
    void readSettings();
    Ui::SettingsDialog *ui;

private slots:
    void applySettings();
    void applySettingsAndClose();
    void bgColorDialog();

signals:
    void settingsChanged();
};

#endif // SETTINGSDIALOG_H
