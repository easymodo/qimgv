#pragma once

#include <QObject>
#include "gui/idirectoryview.h"

class DirectoryViewWrapper : public QObject,
                             public IDirectoryView
{
    Q_OBJECT
public:
    DirectoryViewWrapper(QObject *parent = 0);

public slots:
    void populate(int);
    void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb);
    void selectIndex(int);
    int selectedIndex();
    void focusOn(int);
    void setDirectoryPath(QString path);
    void insertItem(int index);
    void removeItem(int index);
    void reloadItem(int index);

signals:
    void thumbnailPressed(int);
    void thumbnailsRequested(QList<int>, int, bool);

private:
    IDirectoryView *view;

};
