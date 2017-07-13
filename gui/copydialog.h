#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPainter>
#include <QKeyEvent>
#include "gui/customwidgets/overlaywidget.h"
#include "gui/customwidgets/pathselectorwidget.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"

enum CopyDialogMode {
    DIALOG_COPY,
    DIALOG_MOVE
};

class CopyDialog : public OverlayWidget {
    Q_OBJECT
public:
    CopyDialog(QWidget *parent);
    ~CopyDialog();
    void saveSettings();    
    void setDialogMode(CopyDialogMode _mode);
    CopyDialogMode dialogMode();

public slots:
    void show();
    void hide();
signals:
    void copyRequested(QString);
    void moveRequested(QString);

protected:
    void recalculateGeometry();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void requestFileOperation(QString path);
    void requestFileOperation(int fieldNumber);
    void readSettings();

private:
    void createDefaultPaths();
    void createPathWidgets();
    void createShortcuts();
    QGridLayout mLayout;
    QColor bgColor, borderColor;
    QList<PathSelectorWidget*> pathWidgets;
    const int maxPathCount = 9;
    QStringList paths;
    QMap<QString, int> shortcuts;
    QLabel headerLabel;
    CopyDialogMode mode;
};

#endif // COPYDIALOG_H
