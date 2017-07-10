#include "scaler.h"

/* What this should do in theory:
 * 1 request comes
 * 2 we run it
 * 3a if during scaling no new requests came, we return the result and forget about it. end.
 * 3b if some requests did come, by the end of current task we dispose of its result,
 *    start the last task that came and ignore the middle ones.
 */

Scaler::Scaler(Cache *_cache, QObject *parent) : cache(_cache), QObject(parent), currentRequestTimestamp(0), buffered(false), running(false)
{
    runnable = new ScalerRunnable(cache);
    runnable->setAutoDelete(false);
    connect(runnable, SIGNAL(finished(QImage*,ScalerRequest)), this, SLOT(onTaskFinish(QImage*,ScalerRequest)), Qt::QueuedConnection);
}

void Scaler::requestScaled(ScalerRequest req) {
    cache->reserve(req.image->info()->fileName());
    if(!running) {
        startRequest(req);
    } else {
        // something is running. buffer the request
        if(buffered && bufferedRequest.image != req.image) {
            cache->release(bufferedRequest.image->info()->fileName());
        }
        buffered = true;
        bufferedRequest = req;
    }
}

void Scaler::onTaskFinish(QImage *scaled, ScalerRequest req) {
    // clear it; no task is running
    running = false;
    if(!buffered) {
        // no new requests came, cool, return this one
        QPixmap *pixmap = new QPixmap(scaled->size());
        *pixmap = QPixmap::fromImage(*scaled);
        delete scaled;
        emit scalingFinished(pixmap, req);
    } else {
        // new requests came. delete the result. run the buffered
        delete scaled;
        buffered = false;
        startRequest(bufferedRequest);
    }
}

void Scaler::startRequest(ScalerRequest req) {
    running = true;
    runnable->setRequest(req);
    QThreadPool::globalInstance()->start(runnable);
}
