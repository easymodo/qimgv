#include "../private/windowsdirectorywatcher_p.h"

void WindowsWatcherWorker::setDirectoryHandle(HANDLE hDirectory)
{
    this->hDirectory = hDirectory;
}

void WindowsWatcherWorker::run()
{
    while (true)
    {
        WatcherWorker::run();
        WINBOOL result = ReadDirectoryChangesW(
                    hDirectory,
                    &buffer,
                    sizeof(buffer),
                    false,
                    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
                    &bytesReturned,
                    NULL,
                    NULL);

        if (result == 0)
        {
            DWORD errorCode = GetLastError();
            qDebug() << "Something going error:" << errorCode;
            continue;
        }

        if (bytesReturned == 0)
        {
            qDebug() << "Buffer is too low or too big";
            return;
        }

        PFILE_NOTIFY_INFORMATION notify;
        UINT offset = 0;
        PCHAR b = (PCHAR) &buffer;

        do {
            notify = (PFILE_NOTIFY_INFORMATION) (b + offset);
            offset += notify->NextEntryOffset;
            emit notifyEvent(notify);
        } while (notify->NextEntryOffset > 0);
    }
}

QString lastError()
{
    char buffer[1024];
    DWORD lastError = GetLastError();
    int res = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             lastError,
                             LANG_SYSTEM_DEFAULT,
                             buffer,
                             sizeof(buffer),
                             NULL);

    QString line = QString(__FILE__) + "::" + QString::number(__LINE__) + ": ";
    return res == 0 ? QString::number(GetLastError()) : line + buffer;
}

WindowsWatcherPrivate::WindowsWatcherPrivate(WindowsWatcher* qq)
    : DirectoryWatcherPrivate(qq, new WindowsWatcherWorker)
{
    WindowsWatcherWorker* w = (WindowsWatcherWorker*) worker;
    qRegisterMetaType<PFILE_NOTIFY_INFORMATION>("PFILE_NOTIFY_INFORMATION");

    connect(w, SIGNAL(notifyEvent(PFILE_NOTIFY_INFORMATION)),
            this, SLOT(dispatchNotify(PFILE_NOTIFY_INFORMATION)));
}

HANDLE WindowsWatcherPrivate::requestDirectoryHandle(const QString& path)
{
    HANDLE hDirectory;

    do {
        hDirectory = CreateFile(
                    path.toStdString().data(),
                    FILE_LIST_DIRECTORY,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_BACKUP_SEMANTICS,
                    NULL);

        if (hDirectory == INVALID_HANDLE_VALUE)
        {
            if (GetLastError() == ERROR_SHARING_VIOLATION)
            {
                qDebug() << "ERROR_SHARING_VIOLATION waiting for 1 sec";
                QThread::sleep(1);
            }
            else
            {
                qDebug() << lastError();
                return INVALID_HANDLE_VALUE;
            }
        }
    } while (hDirectory == INVALID_HANDLE_VALUE);
    return hDirectory;
}

void WindowsWatcherPrivate::dispatchNotify(PFILE_NOTIFY_INFORMATION notify)
{
    int len = notify->FileNameLength / sizeof(WCHAR);
    QString name = QString::fromWCharArray((wchar_t*) &notify->FileName, len);

    if (!isFileNeeded(name))
        return;

    int pos;

    switch (notify->Action)
    {
        case FILE_ACTION_ADDED:
            onFileCreated(name);
            break;

        case FILE_ACTION_MODIFIED:
            WatcherEvent* event;
            if (findEventIndexByName(name) != -1)
                return;

            event = new WatcherEvent(name, WatcherEvent::MODIFIED);
            event->mTimerId = startTimer(500);
            directoryEvents.push_back(event);
            break;

        case FILE_ACTION_REMOVED:
            onFileDeleted(name);
            break;

        case FILE_ACTION_RENAMED_NEW_NAME:
            onFileRenamed(oldFileName, name);
            break;

        case FILE_ACTION_RENAMED_OLD_NAME:
            oldFileName = name;
            break;

        default:
            qDebug() << "Some error, notify->Action" << notify->Action;
    }
}

WindowsWatcher::WindowsWatcher()
    : DirectoryWatcher(*new WindowsWatcherPrivate(this)) {}

WindowsWatcher::WindowsWatcher(const QString& path)
    : DirectoryWatcher(*new WindowsWatcherPrivate(this))
{
    setPath(path);
}

WindowsWatcher::~WindowsWatcher()
{
}

void WindowsWatcher::setPath(const QString &path)
{
    Q_D(WindowsWatcher);

    HANDLE hDirectory = d->requestDirectoryHandle(path);
    if (hDirectory == INVALID_HANDLE_VALUE)
    {
        qDebug() << "requestDirectoryHandle: INVALID_HANDLE_VALUE";
        return;
    }

    ((WindowsWatcherWorker*)d->worker)->setDirectoryHandle(hDirectory);
    DirectoryWatcher::setWatchPath(path);
}
