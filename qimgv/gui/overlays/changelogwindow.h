#pragma once

#include <QWidget>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QWheelEvent>
#include <QTextBrowser>
#include "gui/customwidgets/overlaywidget.h"
#include "settings.h"
#include "components/actionmanager/actionmanager.h"

namespace Ui {
    class ChangelogWindow;
}

class ChangelogWindow : public OverlayWidget {
    Q_OBJECT
public:
    explicit ChangelogWindow(FloatingWidgetContainer *parent);
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
