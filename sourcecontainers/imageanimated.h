#ifndef IMAGEANIMATED_H
#define IMAGEANIMATED_H

#include "image.h"
#include <QMovie>
#include <QTimer>

class ImageAnimated : public Image {
public:
    ImageAnimated(QString _path);
    ~ImageAnimated();

    QPixmap *getPixmap();
    const QImage* getImage();
    QMovie* getMovie();
    int height();
    int width();
    QSize size();

    bool isEditable();
    bool isEdited();

public slots:
    bool save();
    bool save(QString destPath);

signals:
    void frameChanged(QPixmap*);

private:
    void load();
    QSize mSize;
};

#endif // IMAGEANIMATED_H
