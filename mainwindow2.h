#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QFileDialog>
#include "viewerwidget.h"
#include "actionmanager.h"
#include "settings.h"
#include "settingsdialog.h"
#include "overlays/controlsoverlay.h"
#include "overlays/infooverlay.h"
#include "customWidgets/overlaymessage.h"
#include "thumbnailPanel/thumbnailstrip.h"
#include "customWidgets/mainpanel.h"
#include "customWidgets/slidevpanel.h"
#include "customWidgets/toolbox.h"

class MainWindow2 : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow2(ViewerWidget *viewerWidget, QWidget *parent = nullptr);
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

    ControlsOverlay *controlsOverlay;
    InfoOverlay *infoOverlay;
    OverlayMessage *overlayMessage;

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

public slots:
    void showSaveDialog();
    void showOpenDialog();
    void showSettings();
    void triggerFullscreen();
    void setInfoString(QString);
    void showMessageDirectoryEnd();
    void showMessageDirectoryStart();
    void showMessageFitAll();
    void showMessageFitWidth();
    void showMessageFitOriginal();
};

#endif // MAINWINDOW2_H
