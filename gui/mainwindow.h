#ifndef MainWindow_H
#define MainWindow_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMimeData>
#include "gui/viewers/viewerwidget.h"
#include "gui/overlays/controlsoverlay.h"
#include "gui/overlays/infooverlay.h"
#include "gui/overlays/floatingmessage.h"
#include "gui/panels/mainpanel/thumbnailstrip.h"
#include "gui/panels/mainpanel/mainpanel.h"
#include "gui/panels/sidepanel/toolbox.h"
#include "gui/copydialog.h"
#include "gui/customwidgets/slidevpanel.h"
#include "components/actionmanager/actionmanager.h"
#include "settings.h"
#include "settingsdialog.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(ViewerWidget *viewerWidget, QWidget *parent = nullptr);
    void setPanelWidget(QWidget*);
    bool hasPanelWidget();

private:
    ViewerWidget *viewerWidget;
    void setViewerWidget(ViewerWidget *viewerWidget);
    QHBoxLayout layout;
    QTimer windowMoveTimer;
    int currentDisplay;
    QDesktopWidget *desktopWidget;

    bool mainPanelEnabled, sidePanelEnabled;
    MainPanel *mainPanel;
    SlideVPanel *sidePanel;

    CopyDialog *copyDialog;

    ControlsOverlay *controlsOverlay;
    InfoOverlay *infoOverlay;
    FloatingMessage *floatingMessage;

    PanelHPosition panelPosition;
    PanelVPosition sidePanelPosition;
    QPoint lastMouseMovePos;

    void saveWindowGeometry();
    void restoreWindowGeometry();
    void saveCurrentDisplay();

    void setupOverlays();
    void updateOverlayGeometry();

private slots:
    void updateCurrentDisplay();
    void readSettings();
    void setControlsOverlayEnabled(bool mode);
    void setInfoOverlayEnabled(bool mode);    
    void triggerPanelButtons();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    bool event(QEvent *event);
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *event);
    void resizeEvent(QResizeEvent *event);

signals:
    void opened(QString);
    void fullscreenStatusChanged(bool);
    void copyRequested(QString);
    void moveRequested(QString);

public slots:
    void showDefault();
    void showSaveDialog();
    void showOpenDialog();
    void showSettings();
    void triggerFullScreen();
    void setInfoString(QString);
    void showMessageDirectoryEnd();
    void showMessageDirectoryStart();
    void showMessageFitAll();
    void showMessageFitWidth();
    void showMessageFitOriginal();
    void showFullScreen();
    void showWindowed();
    void triggerCopyDialog();
    void showMessage(QString text);
    void triggerMoveDialog();
    void closeFullScreenOrExit();
    void close();
};

#endif // MainWindow_H
