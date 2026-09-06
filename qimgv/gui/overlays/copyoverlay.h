#pragma once

#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QKeyEvent>
#include "gui/customwidgets/overlaywidget.h"
#include "gui/customwidgets/pathselectormenuitem.h"
#include "gui/customwidgets/actionmenuitem.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"

enum CopyOverlayMode {
    OVERLAY_COPY,
    OVERLAY_MOVE
};

// which list is currently shown for keys 1-9:
// user-configured folders, or subfolders of the current image's directory
enum FolderSource {
    SOURCE_CONFIGURED,
    SOURCE_DISK
};

namespace Ui {
    class CopyOverlay;
}

class CopyOverlay : public OverlayWidget {
    Q_OBJECT
public:
    CopyOverlay(FloatingWidgetContainer *parent);
    ~CopyOverlay();
    void saveSettings();
    void setDialogMode(CopyOverlayMode _mode);
    CopyOverlayMode operationMode();

public slots:
    void show();
    void hide();
    // directory of the currently displayed image; used to list
    // subfolders when the "disk folders" source is active
    void setCurrentDirectory(QString dir);

signals:
    void copyRequested(QString);
    void moveRequested(QString);

protected:
    void keyPressEvent(QKeyEvent *event);

    bool focusNextPrevChild(bool);
private slots:
    void requestFileOperation(QString path);
    void readSettings();
    void toggleFolderSource();

private:
    void createDefaultPaths();
    void createPathWidgets();
    void createShortcuts();
    void syncConfiguredPaths();
    void refreshDiskPaths();
    Ui::CopyOverlay *ui;
    QList<PathSelectorMenuItem*> pathWidgets;
    ActionMenuItem *sourceToggleWidget = nullptr;
    const int maxPathCount = 9;
    QStringList paths;
    QStringList diskPaths;
    QString currentDir;
    QMap<QString, int> shortcuts;
    CopyOverlayMode mode;
    FolderSource folderSource;
    void removePathWidgets();

};
