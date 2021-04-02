#pragma once

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "components/actionmanager/actionmanager.h"
#include "gui/customwidgets/floatingwidget.h"
#include <QDebug>

namespace Ui {
class FVOptionsPopup;
}

class FVOptionsPopup : public QWidget {
    Q_OBJECT

public:
    explicit FVOptionsPopup(QWidget *parent = nullptr);
    ~FVOptionsPopup();

public slots:
    void showAt(QPoint pos);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void hideEvent(QHideEvent *event);

signals:
    void dismissed();
    void viewModeSelected(FolderViewMode);

private slots:
    void readSettings();

    void selectSimpleView();
    void selectExtendedView();
    void selectFoldersView();

private:
    Ui::FVOptionsPopup *ui;
    void setViewMode(FolderViewMode mode);
    void setSimpleView();
    void setExtendedView();
    void setFoldersView();
};
