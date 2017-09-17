#include "scaler.h"

/* What this should do in theory:
 * 1 request comes
 * 2 we run it
 * 3a if during scaling no new requests came, we return the result and forget about it. end.
 * 3b if some requests did come, by the end of current task we dispose of its result,
 *    start the last task that came and ignore the middle ones.
 */

Scaler::Scaler(Cache *_cache, QObject *parent)
    : cache(_cache),
      QObject(parent),
      currentRequestTimestamp(0),
      buffered(false),
      running(false)
{
    sem = new QSemaphore(1);
    pool = new QThreadPool(this);
    pool->setMaxThreadCount(1);
    runnable = new ScalerRunnable(cache);
    runnable->setAutoDelete(false);
    connect(this, SIGNAL(startBufferedRequest()), this, SLOT(slotStartBufferedRequest()), Qt::DirectConnection);
    connect(runnable, SIGNAL(started(ScalerRequest)),
            this, SLOT(onTaskStart(ScalerRequest)), Qt::DirectConnection);
    connect(runnable, SIGNAL(finished(QImage*,ScalerRequest)),
            this, SLOT(onTaskFinish(QImage*,ScalerRequest)), Qt::DirectConnection);
    connect(this, SIGNAL(acceptScalingResult(QImage*,ScalerRequest)), this, SLOT(slotForwardScaledResult(QImage*,ScalerRequest)), Qt::QueuedConnection);
}

void Scaler::requestScaled(ScalerRequest req) {
    sem->acquire(1);
    if(!running) {
//////////////////////////////////
        if(!buffered) {
            bufferedRequest = req;
            buffered = true;
          //qDebug() << "1 requestScaled() - locking..  " <<  req.image->info()->fileName();
            cache->reserve(req.image->info()->fileName());
          //qDebug() << "1 requestScaled() - LOCKED!  " <<  req.image->info()->fileName();
            startRequest(req);
        } else if(bufferedRequest.image != req.image) {
          //qDebug() << "2 requestScaled() - locking...  " <<  req.image->info()->fileName();
            cache->reserve(req.image->info()->fileName());
          //qDebug() << "2 requestScaled() - LOCKED!  " <<  req.image->info()->fileName();
            auto tmp = bufferedRequest;
            bufferedRequest = req;
            buffered = true;
            if(startedRequest.image != tmp.image) {
                cache->release(tmp.image->info()->fileName());
              //qDebug() << "2 requestScaled() - RELEASED!  " <<  tmp.image->info()->fileName();
            }
        } else {
            bufferedRequest = req;
            buffered = true;
        }
//////////////////////////////
    } else {
        if(!buffered) {
            if(req.image != startedRequest.image)
                cache->reserve(req.image->info()->fileName());
            bufferedRequest = req;
            buffered = true;
        } else {
            if(req.image == bufferedRequest.image) {
                bufferedRequest = req;
                buffered = true;
            } else {
                if(bufferedRequest.image != startedRequest.image) {
                    //qDebug() << "4 RELEASING " << bufferedRequest.image->info()->fileName();
                    cache->release(bufferedRequest.image->info()->fileName());
                }
                if(req.image != startedRequest.image)
                    cache->reserve(req.image->info()->fileName());
                bufferedRequest = req;
                buffered = true;
            }
        }
    }
    sem->release(1);
}

void Scaler::onTaskStart(ScalerRequest req) {
    sem->acquire(1);
    running = true;
    // clear buffered flag if there were no requests after us
    if(buffered && bufferedRequest.image == req.image && bufferedRequest.size == req.size) {
        buffered = false;
    }
    startedRequest = req;
  //qDebug() << "onTaskStart(): " << req.image->info()->fileName();
    sem->release(1);
}

void Scaler::onTaskFinish(QImage *scaled, ScalerRequest req) {
    sem->acquire(1);
    running = false;
    if(buffered && bufferedRequest.image == req.image) {
    } else {
      //qDebug() << "onTaskFinish() - 2 releasing..  " <<  req.image->info()->fileName();
        QString name = req.image->info()->fileName();
        cache->release(req.image->info()->fileName());
      //qDebug() << "onTaskFinish() - 2 RELEASED!  " <<  name;
    }
    if(buffered) {
      //qDebug() << "onTaskFinish - startingBuffered: " << bufferedRequest.string;
        delete scaled;
        //startRequest(bufferedRequest);
        emit startBufferedRequest();
        sem->release(1);
    } else {
        sem->release(1);
        emit acceptScalingResult(scaled, req);
    }
}

void Scaler::slotStartBufferedRequest() {
    startRequest(bufferedRequest);
}

void Scaler::slotForwardScaledResult(QImage *image, ScalerRequest req) {
    QPixmap *pixmap = new QPixmap();
    *pixmap = QPixmap::fromImage(*image);
    delete image;
    emit scalingFinished(pixmap, req);
}

void Scaler::startRequest(ScalerRequest req) {
    runnable->setRequest(req);
    pool->start(runnable);
}
