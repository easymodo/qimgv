#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "zzscrollarea.h"
#include "zzlabel.h"
#include "infooverlay.h"
#include "controlsoverlay.h"
#include "mapoverlay.h"
#include <QMainWindow>

#include <time.h>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void openDialog();
    void open(QString);

    void zoomIn();
    void zoomOut();
    void fitImage();
    void fitModeAll();
    void fitModeWidth();
    void fitModeNormal();
    void switchFitMode();
    void switchFullscreen();
    void triggerFullscreen();
    void minimizeWindow();
    void updateMapOverlay();
    
public:
    MainWindow();
    ~MainWindow();
    
private:
    void init();
    void loadMovie(QString);
    void loadStaticImage(QString);
    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateWindowTitle();
    void updateInfoOverlay();

    void keyPressEvent(QKeyEvent *event);
    zzLabel *imgLabel;
    zzScrollArea *scrollArea;
    QMovie *movie;
    QColor bgColor;
    double scaleFactor;

    

    QAction *openAct;
    QAction *nextAct;
    QAction *prevAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *fitModeNormalAct;
    QAction *fitModeAllAct;
    QAction *fitModeWidthAct;
    QAction *fullscreenEnabledAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *navigationMenu;
    QMenu *helpMenu;
    infoOverlay *overlay;
    controlsOverlay *cOverlay;
    mapOverlay *mOverlay;
    bool openFinished;

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *target, QEvent *event);
};

#endif // MAINWINDOW_H
