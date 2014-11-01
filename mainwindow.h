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

public:
    MainWindow();
    ~MainWindow();    
    ImageViewer *imageViewer;
    SettingsDialog *settingsDialog;

    void open(QString path);
private slots:
    void slotOpenDialog();
    void slotFitAll();
    void slotFitWidth();
    void slotFitNormal();
    void switchFitMode();
    void slotSetInfoString(QString info);

    void slotShowControls(bool);
    void slotShowInfo(bool x);
private:
    Core *core;
    textOverlay *infoOverlay, *messageOverlay;
    ControlsOverlay *controlsOverlay;
    void init();
    void createActions();
    void createMenus();
    void keyPressEvent(QKeyEvent *event);
    
    QAction *openAct;
    QAction *settingsAct;
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
    QMenu *viewMenu;
    QMenu *navigationMenu;
    QMenu *helpMenu;

protected:
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *target, QEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
