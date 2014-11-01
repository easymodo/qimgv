#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>

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
    void readSettings();
    Ui::SettingsDialog *ui;

private slots:
    void writeSettings();

signals:
    void settingsChanged();
};

#endif // SETTINGSDIALOG_H
