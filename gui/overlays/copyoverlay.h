#ifndef COPYOVERLAY_H
#define COPYOVERLAY_H

#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QKeyEvent>
#include "gui/customwidgets/floatingwidget.h"
#include "gui/customwidgets/pathselectorwidget.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"

enum CopyOverlayMode {
    OVERLAY_COPY,
    OVERLAY_MOVE
};

namespace Ui {
    class CopyOverlay;
}

class CopyOverlay : public FloatingWidget {
    Q_OBJECT
public:
    CopyOverlay(OverlayContainerWidget *parent);
    ~CopyOverlay();
    void saveSettings();    
    void setDialogMode(CopyOverlayMode _mode);
    CopyOverlayMode operationMode();

public slots:
    void show();
    void hide();

signals:
    void copyRequested(QString);
    void moveRequested(QString);

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void requestFileOperation(QString path);
    void requestFileOperation(int fieldNumber);
    void readSettings();

private:
    void createDefaultPaths();
    void createPathWidgets();
    void createShortcuts();
    Ui::CopyOverlay *ui;
    QList<PathSelectorWidget*> pathWidgets;
    const int maxPathCount = 9;
    QStringList paths;
    QMap<QString, int> shortcuts;
    CopyOverlayMode mode;
    void removePathWidgets();
};

#endif // COPYOVERLAY_H
