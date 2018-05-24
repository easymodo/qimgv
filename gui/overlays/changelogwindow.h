#ifndef CHANGELOGWINDOW_H
#define CHANGELOGWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QWheelEvent>
#include <QTextBrowser>
#include "gui/customwidgets/floatingwidget.h"
#include "settings.h"

namespace Ui {
    class ChangelogWindow;
}

class ChangelogWindow : public FloatingWidget {
    Q_OBJECT
public:
    explicit ChangelogWindow(OverlayContainerWidget *parent);
    ~ChangelogWindow();
    void setText(QString text);

protected:
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);
private:
    Ui::ChangelogWindow *ui;

private slots:
    void hideAndShutUp();
};

#endif // CHANGELOGWINDOW_H
