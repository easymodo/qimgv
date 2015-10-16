#include "imagestatic.h"
#include <time.h>

//use this one
ImageStatic::ImageStatic ( QString _path )
{
     path = _path;
     loaded = false;
     image = NULL;
     info=new FileInfo ( path, this );
     sem = new QSemaphore ( 1 );
     unloadRequested = false;
}

ImageStatic::ImageStatic ( FileInfo *_info )
{
     loaded = false;
     image = NULL;
     info=_info;
     path=info->getFilePath();
     sem = new QSemaphore ( 1 );
     unloadRequested = false;
}

ImageStatic::~ImageStatic()
{
     delete image;
     delete info;
}

//load image data from disk
void ImageStatic::load()
{
     QMutexLocker locker ( &mutex );
     if ( !info )
          info = new FileInfo ( path );
     if ( isLoaded() ) {
          return;
     }
     image = new QImage ( path, info->getExtension() );
     loaded = true;
}

void ImageStatic::save ( QString destinationPath )
{
     if ( isLoaded() ) {
          lock();
          image->save ( destinationPath, getExtension ( destinationPath ), 100 );
          unlock();
     }
}

void ImageStatic::save()
{
     if ( isLoaded() ) {
          lock();
          image->save ( path, info->getExtension(), 100 );
          unlock();
     }
}

QPixmap* ImageStatic::generateThumbnail()
{
     int size = settings->thumbnailSize();
     QPixmap *thumbnail = new QPixmap ( size, size );
     QPixmap *tmp;
     if ( !isLoaded() ) {
          tmp = new QPixmap ( path, info->getExtension() );
          *tmp = tmp->scaled ( size*2,
                               size*2,
                               Qt::KeepAspectRatioByExpanding,
                               Qt::FastTransformation )
                 .scaled ( size,
                           size,
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation );
     } else {
          tmp = new QPixmap();
          if ( !image->isNull() ) {
               lock();
               *tmp = QPixmap::fromImage (
                           image->scaled ( size*2,
                                           size*2,
                                           Qt::KeepAspectRatioByExpanding,
                                           Qt::FastTransformation )
                           .scaled ( size,
                                     size,
                                     Qt::KeepAspectRatioByExpanding,
                                     Qt::SmoothTransformation ) );
               unlock();
          }
     }
     QRect target ( 0, 0, size,size );
     target.moveCenter ( tmp->rect().center() );
     *thumbnail = tmp->copy ( target );
     delete tmp;
     return thumbnail;
}

QPixmap* ImageStatic::getPixmap()
{
     QPixmap *pix = new QPixmap();
     if ( isLoaded() ) {
          lock();
          pix->convertFromImage ( *image );
          unlock();
     }
     return pix;
}

const QImage* ImageStatic::getImage()
{
     return image;
}

int ImageStatic::height()
{
     return isLoaded() ? image->height() : 0;
}

int ImageStatic::width()
{
     return isLoaded() ? image->width() : 0;
}

QSize ImageStatic::size()
{
     return isLoaded() ? image->size() : QSize ( 0,0 );
}

QImage* ImageStatic::rotated ( int grad )
{
     if ( isLoaded() ) {
          lock();
          QImage *img = new QImage();
          QTransform transform;
          transform.rotate ( grad );
          *img = image->transformed ( transform, Qt::SmoothTransformation );
          unlock();
          return img;
     }
     return NULL;
}

void ImageStatic::rotate ( int grad )
{
     if ( isLoaded() ) {
          QImage *img = rotated ( grad );
          delete image;
          lock();
          image = img;
          unlock();
     }
}

void ImageStatic::crop ( QRect newRect )
{
     if ( isLoaded() ) {
          lock();
          QImage *tmp = new QImage ( newRect.size(), image->format() );
          *tmp = image->copy ( newRect );
          delete image;
          image = tmp;
          unlock();
     }
}

QImage* ImageStatic::cropped ( QRect newRect, QRect targetRes, bool upscaled )
{
     if ( isLoaded() ) {
          lock();
          QImage *cropped = new QImage ( targetRes.size(), image->format() );
          if ( upscaled ) {
               QImage temp = image->copy ( newRect );
               *cropped = temp.scaled ( targetRes.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );
               QRect target ( QPoint ( 0,0 ), targetRes.size() );
               target.moveCenter ( cropped->rect().center() );
               *cropped = cropped->copy ( target );
          } else {
               newRect.moveCenter ( image->rect().center() );
               *cropped = image->copy ( newRect );
          }
          unlock();
          return cropped;
     }
     return NULL;
}
