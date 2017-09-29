#include <QDebug>
#include "watcherevent.h"

WatcherEvent::WatcherEvent(const QString &name, int timerId, WatcherEvent::Type type) :
    mName(name),
    mTimerId(timerId),
    mType(type)
{
}

WatcherEvent::WatcherEvent(const QString& name, uint cookie, int timerId, Type type) :
    mName(name),
    mCookie(cookie),
    mTimerId(timerId),
    mType(type)
{
}

WatcherEvent::~WatcherEvent() {
}

QString WatcherEvent::name() const {
    return mName;
}

void WatcherEvent::setName(const QString &name) {
    mName = name;
}

WatcherEvent::Type WatcherEvent::type() const {
    return mType;
}

void WatcherEvent::setType(WatcherEvent::Type type) {
    mType = type;
}

int WatcherEvent::timerId() const {
    return mTimerId;
}

void WatcherEvent::setTimerId(int timerId) {
    mTimerId = timerId;
}

uint WatcherEvent::cookie() const {
    return mCookie;
}

void WatcherEvent::setCookie(uint cookie) {
    mCookie = cookie;
}
