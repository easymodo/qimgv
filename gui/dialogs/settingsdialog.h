#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QThreadPool>
#include <QTableWidget>
#include <QTextBrowser>
#include <QListWidget>
#include <QStackedWidget>
#include <QGridLayout>
#include <QDebug>
#include "gui/customwidgets/clickablelabel.h"
#include "gui/dialogs/shortcutcreatordialog.h"
#include "gui/dialogs/scripteditordialog.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

public slots:
    int exec();
private:
    QPalette windowColorPalette, fullscreenColorPalette, accentLabelPalette;
    void readSettings();
    void populateShortcuts();
    void populateScripts();
    Ui::SettingsDialog *ui;
    enum Constants {
        thumbSizeSmall = 170,
        thumbSizeMedium = 210,
        thumbSizeLarge = 230,
        thumbSizeVeryLarge = 250
    };

    unsigned int thumbSizeCustom;
    void applyShortcuts();
    void addShortcutToTable(const QString &action, const QString &shortcut);
    void addScriptToList(const QString &name);

    void setupSidebar();
    void removeShortcutAt(int row);
private slots:
    void applySettings();
    void applySettingsAndClose();
    void windowColorDialog();
    void accentColorDialog();

    void addScript();
    void editScript();
    void editScript(QListWidgetItem *item);
    void editScript(QString name);
    void removeScript();

    void addShortcut();
    void removeShortcut();
    void resetShortcuts();
    void selectMpvPath();
    void onMaxZoomSliderChanged(int value);
    void onMaxZoomResolutionSliderChanged(int value);
    void onBgOpacitySliderChanged(int value);
    void onThumbnailerThreadsSliderChanged(int value);
    void fullscreenColorDialog();
signals:
    void settingsChanged();
};

#endif // SETTINGSDIALOG_H
