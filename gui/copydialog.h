#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPainter>
#include "gui/customwidgets/overlaywidget.h"
#include "gui/customwidgets/pathselectorwidget.h"
#include "settings.h"

class CopyDialog : public OverlayWidget {
    Q_OBJECT
public:
    CopyDialog(QWidget *parent);
    ~CopyDialog();
    void saveSettings();

signals:
    void copyRequested(QString);

protected:
    void recalculateGeometry();
    void paintEvent(QPaintEvent *event);

private slots:
    void requestCopy(QString path, int number);
    void readSettings();

private:
    QGridLayout mLayout;
    QPen borderPen;
    QColor bgColor;
    void createPathWidgets();
    QList<PathSelectorWidget*> pathWidgets;
    int maxPathCount;
    QStringList paths;
    void createDefaultPaths();
};

#endif // COPYDIALOG_H
