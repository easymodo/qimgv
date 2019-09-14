#ifndef IDIRECTORYVIEW_H
#define IDIRECTORYVIEW_H

#include <QList>
#include <memory>

class Thumbnail;
class QString;

class IDirectoryView {
public:
    virtual void populate(int) = 0;
    virtual void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) = 0;
    virtual void selectIndex(int) = 0;
    virtual void focusOn(int) = 0;
    virtual int selectedIndex() = 0;
    virtual void setDirectoryPath(QString path) = 0;
    virtual void insertItem(int index) = 0;
    virtual void removeItem(int index) = 0;

//signals
    virtual void thumbnailPressed(int) = 0;
    virtual void thumbnailsRequested(QList<int>, int) = 0;
};

#endif // IDIRECTORYVIEW_H
