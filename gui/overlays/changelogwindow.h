#ifndef CHANGELOGWINDOW_H
#define CHANGELOGWINDOW_H

#include <QWidget>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QWheelEvent>
#include <QTextBrowser>
#include "gui/customwidgets/floatingwidget.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"

namespace Ui {
    class ChangelogWindow;
}

class ChangelogWindow : public FloatingWidget {
    Q_OBJECT
public:
    explicit ChangelogWindow(OverlayContainerWidget *parent);
    ~ChangelogWindow();
    void setText(QString text);

public slots:
    void show();
    void hide();

protected:
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);
    void keyPressEvent(QKeyEvent *);

private:
    Ui::ChangelogWindow *ui;

private slots:
    void hideAndShutUp();
};

#endif // CHANGELOGWINDOW_H
