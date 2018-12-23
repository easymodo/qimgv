#include "scaler.h"

/* What this should do in theory:
 * 1 request comes
 * 2 we run it
 * 3a if during scaling no new requests came, we return the result and forget about it. end.
 * 3b if some requests did come, by the end of current task we dispose of its result,
 *    start the last task that came and ignore the middle ones.
 */

// --------------------------------------
// what the *&$%#   - me, 1 year later

Scaler::Scaler(Cache *_cache, QObject *parent)
    : QObject(parent),
      buffered(false),
      running(false),
      currentRequestTimestamp(0),
      cache(_cache)
{
    sem = new QSemaphore(1);
    pool = new QThreadPool(this);
    pool->setMaxThreadCount(1);
    runnable = new ScalerRunnable();
    runnable->setAutoDelete(false);
    connect(this, SIGNAL(startBufferedRequest()), this, SLOT(slotStartBufferedRequest()), Qt::DirectConnection);
    connect(runnable, SIGNAL(started(ScalerRequest)),
            this, SLOT(onTaskStart(ScalerRequest)), Qt::DirectConnection);
    connect(runnable, SIGNAL(finished(std::shared_ptr<StaticImageContainer>,ScalerRequest)),
            this, SLOT(onTaskFinish(std::shared_ptr<StaticImageContainer>,ScalerRequest)), Qt::DirectConnection);
    connect(this, SIGNAL(acceptScalingResult(std::shared_ptr<StaticImageContainer>,ScalerRequest)),
            this, SLOT(slotForwardScaledResult(std::shared_ptr<StaticImageContainer>,ScalerRequest)), Qt::QueuedConnection);
}

void Scaler::requestScaled(ScalerRequest req) {
    sem->acquire(1);
    if(!running) {
//////////////////////////////////
        if(!buffered) {
            bufferedRequest = req;
            buffered = true;
            cache->reserve(req.image->name());
            startRequest(req);
        } else if(bufferedRequest.image != req.image) {
            cache->reserve(req.image->name());
            auto tmp = bufferedRequest;
            bufferedRequest = req;
            buffered = true;
            if(startedRequest.image != tmp.image) {
                cache->release(tmp.image->name());
            }
        } else {
            bufferedRequest = req;
            buffered = true;
        }
//////////////////////////////
    } else {
        if(!buffered) {
            if(req.image != startedRequest.image)
                cache->reserve(req.image->name());
            bufferedRequest = req;
            buffered = true;
        } else {
            if(req.image == bufferedRequest.image) {
                bufferedRequest = req;
                buffered = true;
            } else {
                if(bufferedRequest.image != startedRequest.image) {
                    cache->release(bufferedRequest.image->name());
                }
                if(req.image != startedRequest.image)
                    cache->reserve(req.image->name());
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
    sem->release(1);
}

// called from off-thread
void Scaler::onTaskFinish(std::shared_ptr<StaticImageContainer> scaled, ScalerRequest req) {
    sem->acquire(1);
    running = false;
    if( !(buffered && bufferedRequest.image == req.image) ) {
        cache->release(req.image->name());
    }
    if(buffered) {
        emit startBufferedRequest();
        sem->release(1);
    } else {
        sem->release(1);
        // convert & return pixmap in main thread
        emit acceptScalingResult(scaled, req);
    }
}

void Scaler::slotStartBufferedRequest() {
    startRequest(bufferedRequest);
}

void Scaler::slotForwardScaledResult(std::shared_ptr<StaticImageContainer> image, ScalerRequest req) {
    QPixmap *pixmap = new QPixmap();
    *pixmap = QPixmap::fromImage(*image->getImage());
    emit scalingFinished(pixmap, req);
}

void Scaler::startRequest(ScalerRequest req) {
    if(req.size != req.image->size()) {
        runnable->setRequest(req);
        pool->start(runnable);
    } else { // request is the same size as original; straight convert to QPixmap & return
        if(buffered && bufferedRequest.image == req.image && bufferedRequest.size == req.size) {
            buffered = false;
        }
        startedRequest = req;
        QPixmap *result = new QPixmap();
        result->convertFromImage(*req.image->getImage());
        cache->release(req.image->name());
        running = false;
        if( !(buffered && bufferedRequest.image == req.image) ) {
            cache->release(req.image->name());
        }
        emit scalingFinished(result, req);
    }
}
