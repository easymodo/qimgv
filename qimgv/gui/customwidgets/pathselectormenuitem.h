#pragma once

#include <QFileDialog>
#include "gui/customwidgets/menuitem.h"
#include "settings.h"

class PathSelectorMenuItem : public MenuItem {
    Q_OBJECT
public:
    PathSelectorMenuItem(QWidget *parent);
    QString directory();

public slots:
    void setDirectory(QString path);

protected:
    virtual void onPress() Q_DECL_OVERRIDE;

signals:
    void directorySelected(QString);

private:
    QString mDirectory;
};
