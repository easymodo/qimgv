#include "scaler.h"

/* What this should do in theory:
 * 1 request comes
 * 2 we run it
 * 3a if during scaling no new requests came, we return the result and forget about it. end.
 * 3b if some requests did come, by the end of current task we dispose of its result,
 *    start the last task that came and ignore the middle ones.
 */

Scaler::Scaler(QObject *parent) : QObject(parent), currentRequestTimestamp(0), buffered(false), running(false)
{
    runnable.setAutoDelete(false);
    connect(&runnable, SIGNAL(finished(QImage*,ScalerRequest)), this, SLOT(onTaskFinish(QImage*,ScalerRequest)), Qt::QueuedConnection);
}

void Scaler::requestScaled(ScalerRequest req) {
    requestMutex.lock();
    req.image->lock();
    if(!running) {
        startRequest(req);
    } else {
        // something is running. buffer the request
        if(buffered)
            bufferedRequest.image->unlock();
        buffered = true;
        bufferedRequest = req;
    }
    requestMutex.unlock();
}

void Scaler::onTaskFinish(QImage *scaled, ScalerRequest req) {
    requestMutex.lock();
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
    requestMutex.unlock();
}

void Scaler::startRequest(ScalerRequest req) {
    running = true;
    runnable.setRequest(req);
    QThreadPool::globalInstance()->start(&runnable);
}
