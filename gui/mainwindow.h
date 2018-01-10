#ifndef MainWindow_H
#define MainWindow_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMimeData>
#include "gui/customwidgets/containerwidget.h"
#include "gui/viewers/viewerwidget.h"
#include "gui/overlays/controlsoverlay.h"
#include "gui/overlays/infooverlay.h"
#include "gui/overlays/floatingmessage.h"
#include "gui/overlays/saveconfirmoverlay.h"
#include "gui/panels/mainpanel/thumbnailstrip.h"
#include "gui/panels/mainpanel/mainpanel.h"
#include "gui/panels/sidepanel/sidepanel.h"
#include "gui/panels/croppanel/croppanel.h"
#include "gui/overlays/cropoverlay.h"
#include "gui/copydialog.h"
#include "gui/resizedialog.h"
#include "components/actionmanager/actionmanager.h"
#include "settings.h"
#include "settingsdialog.h"

enum ActiveSidePanel {
    SIDEPANEL_CROP,
    SIDEPANEL_NONE
};

class MainWindow : public ContainerWidget
{
    Q_OBJECT
public:
    explicit MainWindow(ViewerWidget *viewerWidget, QWidget *parent = nullptr);
    void setPanelWidget(QWidget*);
    bool hasPanelWidget();
    bool isCropPanelActive();

private:
    ViewerWidget *viewerWidget;
    void setViewerWidget(ViewerWidget *viewerWidget);
    QHBoxLayout layout;
    QTimer windowMoveTimer;
    int currentDisplay;
    QDesktopWidget *desktopWidget;

    bool panelEnabled, panelFullscreenOnly, cropPanelActive;
    ActiveSidePanel activeSidePanel;
    MainPanel *mainPanel;
    SidePanel *sidePanel;
    CropPanel *cropPanel;
    CropOverlay *cropOverlay;
    SaveConfirmOverlay *saveOverlay;

    CopyDialog *copyDialog;

    ControlsOverlay *controlsOverlay;
    InfoOverlay *infoOverlay;
    FloatingMessage *floatingMessage;

    PanelHPosition panelPosition;
    QPoint lastMouseMovePos;

    void saveWindowGeometry();
    void restoreWindowGeometry();
    void saveCurrentDisplay();
    void setupOverlays();

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
    void resizeRequested(QSize);
    void cropRequested(QRect);
    void discardEditsRequested();
    void saveAsClicked();
    void saveRequested();
    void saveRequested(QString);

public slots:
    void showDefault();
    void showCropPanel();
    void hideSidePanel();
    void showOpenDialog();
    void showSaveDialog(QString filePath);
    void showResizeDialog(QSize initialSize);
    void showSettings();
    void triggerFullScreen();
    void setInfoString(QString);
    void showMessageDirectoryEnd();
    void showMessageDirectoryStart();
    void showMessageFitWindow();
    void showMessageFitWidth();
    void showMessageFitOriginal();
    void showFullScreen();
    void showWindowed();
    void triggerCopyDialog();
    void showMessage(QString text);
    void triggerMoveDialog();
    void closeFullScreenOrExit();
    void close();
    void triggerCropPanel();
    void setupSidePanelData();
    void showSaveOverlay();
    void hideSaveOverlay();
};

#endif // MainWindow_H
