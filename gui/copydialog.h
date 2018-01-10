#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPainter>
#include <QKeyEvent>
#include "gui/customwidgets/floatingwidget.h"
#include "gui/customwidgets/pathselectorwidget.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"

enum CopyDialogMode {
    DIALOG_COPY,
    DIALOG_MOVE
};

namespace Ui {
    class CopyDialog;
}

class CopyDialog : public FloatingWidget {
    Q_OBJECT
public:
    CopyDialog(ContainerWidget *parent);
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
    void keyPressEvent(QKeyEvent *event);

private slots:
    void requestFileOperation(QString path);
    void requestFileOperation(int fieldNumber);
    void readSettings();

private:
    void createDefaultPaths();
    void createPathWidgets();
    void createShortcuts();
    Ui::CopyDialog *ui;
    QList<PathSelectorWidget*> pathWidgets;
    const int maxPathCount = 9;
    QStringList paths;
    QMap<QString, int> shortcuts;
    CopyDialogMode mode;
    void removePathWidgets();
};

#endif // COPYDIALOG_H
