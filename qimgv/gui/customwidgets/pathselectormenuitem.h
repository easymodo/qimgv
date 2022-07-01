#pragma once

#include <QFileDialog>
#include "gui/customwidgets/menuitem.h"
#include "settings.h"

class PathSelectorMenuItem : public MenuItem {
    Q_OBJECT
public:
    PathSelectorMenuItem(QWidget *parent);
    QString directory();
    QString path();

public slots:
    void setDirectory(QString path);

protected:
    virtual void onPress() override;

signals:
    void directorySelected(QString);

private:
    QString mDirectory;
    QString mPath;
};
