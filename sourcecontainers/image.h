#ifndef IMAGE_H
#define IMAGE_H

#include <QString>
#include <QDebug>
#include <QPixmap>
#include <memory>
#include "utils/imagelib.h"
#include "utils/stuff.h"
#include "sourcecontainers/documentinfo.h"

class Image {
public:
    Image(QString);
    Image(std::unique_ptr<DocumentInfo>);
    virtual ~Image() = 0;
    virtual std::unique_ptr<QPixmap> getPixmap() = 0;
    virtual std::shared_ptr<const QImage> getImage() = 0;
    DocumentType type() const;
    QString path() const;
    virtual int height() = 0;
    virtual int width() = 0;
    virtual QSize size() = 0;
    bool isLoaded() const;
    virtual bool save() = 0;
    virtual bool save(QString destPath) = 0;
    QString name() const;
    bool isEdited() const;
    int fileSize() const;

protected:
    virtual void load() = 0;
    std::unique_ptr<DocumentInfo> mDocInfo;
    bool mLoaded, mEdited;
    QString mPath;
    QSize resolution;
};

#endif // IMAGE_H
