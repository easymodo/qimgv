#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #include "scrollarea.h"

#include <QMainWindow>
#include <QtWidgets>
#include <QAction>

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
    void slotFitAll();
    void slotFitWidth();
    void slotFitNormal();
    void slotFullscreen();
    void slotTriggerFullscreen();
    void slotMinimize();
    void slotOpenDialog();
    void slotNextImage();
    void slotPrevImage();
    void slotZoomIn();
    void slotZoomOut();

signals:
    void signalFitAll();
    void signalFitWidth();
    void signalFitNormal();
    void signalOpenDialog();
    void signalNextImage();
    void signalPrevImage();
    void signalZoomIn();
    void signalZoomOut();

public:
    MainWindow();
    ~MainWindow();

    
private:
    void init();
    void createActions();
    void createMenus();
    void updateActions();
    void keyPressEvent(QKeyEvent *event);
    
    QColor bgColor;
    double scaleFactor;
    QAction *openAct;
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
};

#endif // MAINWINDOW_H
