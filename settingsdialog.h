#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QColorDialog>
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
    static constexpr int thumbSizeSmall = 120,
              thumbSizeMedium = 135,
              thumbSizeLarge = 170,
              thumbSizeVeryLarge = 200;

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
