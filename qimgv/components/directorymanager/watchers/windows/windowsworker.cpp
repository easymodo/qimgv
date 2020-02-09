#include "windowsworker.h"

WindowsWorker::WindowsWorker() : WatcherWorker() {

}

void WindowsWorker::setDirectoryHandle(HANDLE hDir) {
    //qDebug() << "setHandle" << this->hDir << " -> " << hDir;
    this->hDir = hDir;
}

void WindowsWorker::run() {
    running = true;
    DWORD error = 0;
    bool bPending = false;
    DWORD dwBytes = 0;
    OVERLAPPED ovl = {0};
    std::vector<BYTE> buffer(1024*64);

    ovl.hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);

    if(!ovl.hEvent) {
        qDebug() << "[WindowsWorker] CreateEvent failed?";
    }

    ::ResetEvent(ovl.hEvent); // is this needed?

    while(running) {
        //qDebug() << "_1";
        bPending = ReadDirectoryChangesW(hDir,
                                         &buffer[0],
                                         buffer.size(),
                                         FALSE,
                                         FILE_ACTION_ADDED | FILE_ACTION_REMOVED | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_ACTION_MODIFIED,
                                         &dwBytes,
                                         &ovl,
                                         nullptr);
        //qDebug() << "_2";
        if(!bPending) {
            error = GetLastError();
            if(error == ERROR_IO_INCOMPLETE) {
                qDebug() << "ERROR_IO_INCOMPLETE";
                continue;
            }
        }
        //qDebug() << "_3";
        bool WAIT = false;
        if(GetOverlappedResult(hDir, &ovl, &dwBytes, WAIT)) {
            bPending = false;
            if(dwBytes != 0) {
                FILE_NOTIFY_INFORMATION *fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);
                do {
                    if(fni->Action != 0) {
                        emit notifyEvent(fni);
                    }
                    if(fni->NextEntryOffset == 0)
                        break;
                    fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<PCHAR>(fni) + fni->NextEntryOffset);
                } while (true);
            }
        }
        Sleep(POLL_RATE_MS);
    }
}
