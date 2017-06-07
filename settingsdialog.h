#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QThreadPool>
#include <QDebug>
#include "customWidgets/clickablelabel.h"
#include "customWidgets/settingsshortcutwidget.h"
#include "settings.h"
#include "actionmanager.h"

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
    QPalette bgLabelPalette, accentLabelPalette;
    void readSettings();
    void fillShortcuts();
    Ui::SettingsDialog *ui;
    enum Constants {
        thumbSizeSmall = 130,
        thumbSizeMedium = 160,
        thumbSizeLarge = 190,
        thumbSizeVeryLarge = 250
    };


    int thumbSizeCustom;
    QStringList shortcutKeys;

    void applyShortcuts();
private slots:
    void applySettings();
    void applySettingsAndClose();
    void bgColorDialog();

    void accentColorDialog();
    void removeShortcut();
    void addShortcut();
    void resetShortcuts();
    void selectFFMPEG();
signals:
    void settingsChanged();
};

#endif // SETTINGSDIALOG_H
