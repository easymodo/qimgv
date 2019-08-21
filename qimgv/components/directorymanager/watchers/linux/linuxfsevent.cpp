#include <QDebug>

#include "linuxfsevent.h"

LinuxFsEvent::LinuxFsEvent(char *data, uint dataSize) :
    mData(data),
    mDataSize(dataSize)
{
}

LinuxFsEvent::~LinuxFsEvent() {
    delete[] mData;
}

uint LinuxFsEvent::dataSize() const {
    return mDataSize;
}

void LinuxFsEvent::setDataSize(uint bufferSize) {
    mDataSize = bufferSize;
}

char *LinuxFsEvent::data() const {
    return mData;
}

void LinuxFsEvent::setData(char *buffer) {
    mData = buffer;
}
