#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QThreadPool>
#include <QDebug>
#include "gui/customwidgets/clickablelabel.h"
#include "gui/customwidgets/settingsshortcutwidget.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"

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
        thumbSizeSmall = 170,
        thumbSizeMedium = 210,
        thumbSizeLarge = 230,
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
    void selectMpvPath();
    void onMaxZoomSliderChanged(int value);
    void onMaxZoomResolutionSliderChanged(int value);
signals:
    void settingsChanged();
};

#endif // SETTINGSDIALOG_H
