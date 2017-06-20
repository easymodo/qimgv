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

class CopyDialog : public OverlayWidget {
    Q_OBJECT
public:
    CopyDialog(QWidget *parent);
    ~CopyDialog();
    void saveSettings();

public slots:
    void show();
    void hide();
signals:
    void copyRequested(QString);

protected:
    void recalculateGeometry();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void requestCopy(QString path);
    void requestCopy(int fieldNumber);
    void readSettings();

private:
    void createDefaultPaths();
    void createPathWidgets();
    void createShortcuts();
    QGridLayout mLayout;
    QPen borderPen;
    QColor bgColor;
    QList<PathSelectorWidget*> pathWidgets;
    int maxPathCount;
    QStringList paths;
    QMap<QString, int> shortcuts;
};

#endif // COPYDIALOG_H
