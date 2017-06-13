#include "scaler.h"

/* What this should do in theory:
 * 1 request comes
 * 2 we run it
 * 3a if during scaling no new requests came, we return the result and forget about it. end.
 * 3b if some requests did come, by the end of current task we dispose of its result,
 *    start the last task that came and ignore the middle ones.
 *
 * The requests are done one at a time.
 * We do only one request at a time to prevent shitting up the pc resources in case when user sends too many requests in a limited time.
 *
 * The other alternative has very big peak ram usage (when for example 4 requests would scale some huge image simultaneously). We do not want that.
 *
 */

Scaler::Scaler(QObject *parent) : QObject(parent), currentRequest(NULL), nextRequest(NULL)
{
    runnable.setAutoDelete(false);
    connect(&runnable, SIGNAL(finished(QImage*,ScalerRequest*)), this, SLOT(onTaskFinish(QImage*,ScalerRequest*)), Qt::QueuedConnection);
}

void Scaler::requestScaled(ScalerRequest *req) {
    requestMutex.lock();

    // delete any request that came before us and havent run yet
    if(nextRequest) {
        delete nextRequest;
        nextRequest = NULL;
    }

    // fresh start
    if(!currentRequest) {
        currentRequest = req;
        startRequest(currentRequest);
    } else {
        // something is running. we set nextRequest
        // then, current task's onTaskFinish() will run it
        nextRequest = req;
    }

    requestMutex.unlock();
}

void Scaler::onTaskFinish(QImage *scaled, ScalerRequest *req) {
    requestMutex.lock();

    // clear it; no task is running
    currentRequest = NULL;
    // no new requests came, cool, return this one
    if(!nextRequest) {
        QPixmap *pixmap = new QPixmap();
        *pixmap = QPixmap::fromImage(*scaled);
        delete scaled;
        emit scalingFinished(pixmap, req);
    } else {
        // new requests came. delete the result. run the next one
        delete scaled;
        delete req;

        currentRequest = nextRequest;
        nextRequest = NULL;
        startRequest(currentRequest);
    }

    requestMutex.unlock();
}

void Scaler::startRequest(ScalerRequest *req) {
    currentRequest = req;
    runnable.setRequest(req);
    QThreadPool::globalInstance()->start(&runnable);
}
