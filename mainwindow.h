#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFileDialog>
#include <QAction>
#include "core.h"
#include "infooverlay.h"
#include "controlsoverlay.h"
#include "settingsdialog.h"
#include "imageviewer.h"
#include "settings.h"
#include "thumbnailstrip/thumbnailstrip.h"

QT_BEGIN_NAMESPACE
    class QAction;
    class QLabel;
    class QMenu;
    class QScrollArea;
    class QScrollBar;
QT_END_NAMESPACE

class Core;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void slotFullscreen();
    void slotTriggerFullscreen();
    void slotMinimize();
    void slotAbout();
    void setInfoString(QString);
    void readSettings();

    void close();
signals:
    void signalFitAll();
    void signalFitWidth();
    void signalFitNormal();
    void signalNextImage();
    void signalPrevImage();
    void signalZoomIn();
    void signalZoomOut();
    void signalFullscreenEnabled(bool);
    void fileOpened(QString);
    void fileSaved(QString);

public:
    MainWindow();
    ~MainWindow();    
    ImageViewer *imageViewer;
    SettingsDialog *settingsDialog;

    void open(QString path);
    void triggerMenuBar();
    void showMenuBar();

private slots:
    void slotOpenDialog();
    void slotFitAll();
    void slotFitWidth();
    void slotFitNormal();
    void switchFitMode();
    void slotSetInfoString(QString info);
    void slotRotateLeft();
    void slotRotateRight();
    void slotSaveDialog();

    void slotShowControls(bool);
    void slotShowInfo(bool x);
private:
    Core *core;
    textOverlay *infoOverlay, *messageOverlay;
    ControlsOverlay *controlsOverlay;
    ThumbnailStrip *panel;
    void init();
    void createActions();
    void createMenus();
    void saveWindowGeometry();
    void restoreWindowGeometry();
    void keyPressEvent(QKeyEvent *event);
    
    QAction *openAct;
    QAction *saveAct;
    QAction *settingsAct;
    QAction *rotateLeftAct;
    QAction *rotateRightAct;
    QAction *cropAct;
    QAction *nextAct;
    QAction *prevAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *modeFitNormal;
    QAction *modeFitAll;
    QAction *modeFitWidth;
    QAction *fullscreenEnabledAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *navigationMenu;
    QMenu *helpMenu;

    void readSettingsInitial();
protected:
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *target, QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
