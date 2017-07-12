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
    pool = new QThreadPool(this);
    pool->setMaxThreadCount(1);
    runnable = new ScalerRunnable(cache);
    runnable->setAutoDelete(false);
    connect(this, SIGNAL(startBufferedRequest()), this, SLOT(slotStartBufferedRequest()), Qt::QueuedConnection);
    connect(runnable, SIGNAL(started(ScalerRequest)),
            this, SLOT(onTaskStart(ScalerRequest)), Qt::DirectConnection);
    connect(runnable, SIGNAL(finished(QImage*,ScalerRequest)),
            this, SLOT(onTaskFinish(QImage*,ScalerRequest)), Qt::DirectConnection);
    connect(this, SIGNAL(acceptScalingResult(QImage*,ScalerRequest)), this, SLOT(slotForwardScaledResult(QImage*,ScalerRequest)), Qt::QueuedConnection);
}

void Scaler::requestScaled(ScalerRequest req) {
    mutex.lock();
    if(!running) {
        if(!buffered) {
            buffered = true;
            bufferedRequest = req;
            cache->reserve(req.image->info()->fileName());
            startRequest(req);
        } else if(bufferedRequest.image != req.image) {
            cache->reserve(req.image->info()->fileName());
            buffered = true;
            bufferedRequest = req;
        } else {
            buffered = true;
            bufferedRequest = req;
        }
    } else {
        if(startedRequest.image != req.image) {
            cache->reserve(req.image->info()->fileName());
        }
        bufferedRequest = req;
        buffered = true;
    }
    mutex.unlock();
}

void Scaler::onTaskStart(ScalerRequest req) {
    mutex.lock();
    running = true;
    // clear buffered flag if there were no requests after us
    if(buffered && bufferedRequest.image == req.image && bufferedRequest.size == req.size) {
        buffered = false;
    }
    startedRequest = req;
    mutex.unlock();
}

void Scaler::onTaskFinish(QImage *scaled, ScalerRequest req) {
    mutex.lock();
    running = false;
    if(buffered && bufferedRequest.image == req.image) {
    } else {
        cache->release(req.image->info()->fileName());
    }
    if(buffered) {
        delete scaled;
        emit startBufferedRequest();
    } else {
        emit acceptScalingResult(scaled, req);
    }
    mutex.unlock();
}

void Scaler::slotStartBufferedRequest() {
    startRequest(bufferedRequest);
}

void Scaler::slotForwardScaledResult(QImage *image, ScalerRequest req) {
    QPixmap *pixmap = new QPixmap();
    *pixmap = QPixmap::fromImage(*image);
    delete image;
    //qDebug() << "accepting scaled result: " << pixmap->size();
    emit scalingFinished(pixmap, req);
}

void Scaler::startRequest(ScalerRequest req) {
    runnable->setRequest(req);
    pool->start(runnable);
}
