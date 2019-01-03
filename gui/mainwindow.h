#ifndef MainWindow_H
#define MainWindow_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMimeData>
#include "gui/customwidgets/overlaycontainerwidget.h"
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
#include "gui/overlays/copyoverlay.h"
#include "gui/overlays/changelogwindow.h"
#include "gui/dialogs/resizedialog.h"
#include "gui/centralwidget.h"
#include "components/actionmanager/actionmanager.h"
#include "settings.h"
#include "gui/dialogs/settingsdialog.h"
#include "gui/viewers/documentwidget.h"
#include <QApplication>

#ifdef USE_KDE_BLUR
#include <KWindowEffects>
#endif

enum ActiveSidePanel {
    SIDEPANEL_CROP,
    SIDEPANEL_NONE
};

class MainWindow : public OverlayContainerWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    bool isCropPanelActive();
    void onScalingFinished(std::unique_ptr<QPixmap>scaled);
    void showImage(std::unique_ptr<QPixmap> pixmap);
    void showAnimation(std::unique_ptr<QMovie> movie);
    void showVideo(Clip *clip);

private:
    std::shared_ptr<ViewerWidget> viewerWidget;
    QHBoxLayout layout;
    QTimer windowMoveTimer;
    int currentDisplay;
    QDesktopWidget *desktopWidget;

    QColor bgColor;
    qreal bgOpacity;
    bool panelEnabled, panelFullscreenOnly, cropPanelActive, infoOverlayEnabled;
    std::shared_ptr<DocumentWidget> docWidget;
    std::shared_ptr<FolderView> folderView;
    std::shared_ptr<CentralWidget> centralWidget;
    ActiveSidePanel activeSidePanel;
    std::shared_ptr<ThumbnailStrip> thumbnailStrip;
    MainPanel *mainPanel;
    SidePanel *sidePanel;
    CropPanel *cropPanel;
    CropOverlay *cropOverlay;
    SaveConfirmOverlay *saveOverlay;
    ChangelogWindow *changelogWindow;

    CopyOverlay *copyOverlay;

    ControlsOverlay *controlsOverlay;
    InfoOverlay *infoOverlay;
    FloatingMessage *floatingMessage;

    PanelHPosition panelPosition;
    QPoint lastMouseMovePos;

    void saveWindowGeometry();
    void restoreWindowGeometry();
    void saveCurrentDisplay();
    void setupUi();

    void applyWindowedBackground();
    void applyFullscreenBackground();
private slots:
    void updateCurrentDisplay();
    void readSettings();
    void setControlsOverlayEnabled(bool mode);
    void showInfoOverlay(bool mode);
    void triggerPanelButtons();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *event);
    void resizeEvent(QResizeEvent *event);
    void leaveEvent(QEvent *event);

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

    // thumbnails
    void thumbnailRequested(QList<int>, int);
    void selectThumbnail(int);
    void thumbnailPressed(int);
    void onThumbnailReady(int, std::shared_ptr<Thumbnail>);
    // viewerWidget
    void scalingRequested(QSize);
    void zoomIn();
    void zoomOut();
    void zoomInCursor();
    void zoomOutCursor();
    void scrollUp();
    void scrollDown();
    void scrollLeft();
    void scrollRight();
    void pauseVideo();
    void stopPlayback();
    void seekVideoRight();
    void seekVideoLeft();
    void frameStep();
    void frameStepBack();
    void enableFolderView();
    void enableDocumentView();
    void setDirectoryPath(QString);
    void closeImage();
    void toggleFolderView();

public slots:
    void showDefault();
    void showCropPanel();
    void hideCropPanel();
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
    void triggerCopyOverlay();
    void showMessage(QString text);
    void showMessage(QString text, int duration);
    void triggerMoveOverlay();
    void closeFullScreenOrExit();
    void close();
    void triggerCropPanel();
    void setupSidePanelData();
    void showSaveOverlay();
    void hideSaveOverlay();
    void showChangelogWindow();
    void showChangelogWindow(QString text);
    void fitWindow();
    void fitWidth();
    void fitOriginal();
    void switchFitMode();
    void populateThumbnailViews(int count);
    void addThumbnail(int index);
    void removeThumbnail(int index);
};

#endif // MainWindow_H
