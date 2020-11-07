#include "../../include/directorychangeswatcher.h"
#include "../../include/directorychangesexception.h"
#include "../../include/directorychangeshandler.h"

#include <fileapi.h>

#include "../../include/windows/utils.h"

#include <directorychangeswatcher_p.h>

using utils::FilesFinder;

namespace api {
void DirectoryChangesWatcher::createData() {
    m_data = std::make_unique<DirectoryChangesWatcherPlatformData>();
}

void DirectoryChangesWatcher::internalStop() {
    if(!isStopped()) {
        m_stopped.store(true);
        SetEvent(m_data->m_terminateEvent);
    }
}

bool DirectoryChangesWatcher::internalStart() {
    return !(m_stopped = false);
}

size_t const EventsSize = 2;
DWORD const Timeout = INFINITE;

void AddDirectories(const std::wstring& path,  std::vector<std::string>& direcroties){
    WIN32_FIND_DATAW fData {};
    FilesFinder finder(path + L"*", fData);

    if(finder.isValid()){
        while (true) {
            if((fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY &&
               wcscmp(fData.cFileName, L".") != 0 &&
               wcscmp(fData.cFileName, L"..") != 0) {
                std::wstring p = path + std::wstring(fData.cFileName);
                if(p.back() != L'/') {
                    p += L'/';
                }
                direcroties.push_back(utils::ws2s(p));
                AddDirectories(p, direcroties);
            }

            auto result = finder.findNext(fData);
            if(!result) {
                break;
            }
        }
    }
}

void OnDirectoryChanged(DWORD errorCode,
                        BYTE* buffer,
                        LPOVERLAPPED overlapped,
                        const std::string& directory,
                        std::vector<std::string>& directories,
                        send_t& sent);

void DirectoryChangesWatcher::run() {
    DWORD idWaitResult;

    HANDLE directoryEvent;
    HANDLE events[EventsSize];

    ZeroMemory(&m_data->m_overlapped,sizeof(m_data->m_overlapped));

    std::string directory = m_directory;
    if(directory.back() != '/') {
        directory += '/';
    }

   AddDirectories(utils::s2ws(directory), m_data->m_watchedDirectories);

    m_data->m_directoryHandle = CreateFileW(utils::s2ws(directory).c_str(), //имя директории
                        FILE_LIST_DIRECTORY,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        nullptr,
                        OPEN_EXISTING,
                        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                        nullptr);

    if(m_data->m_directoryHandle == INVALID_HANDLE_VALUE) {
        throw DirectoryChangesException(static_cast<int>(GetLastError()));
    }

    m_data->m_terminateEvent=CreateEvent(nullptr,TRUE,FALSE,nullptr);

    if(!m_data->m_terminateEvent) {
        throw DirectoryChangesException(static_cast<int>(GetLastError()));
    }

    directoryEvent = CreateEvent(nullptr,TRUE,FALSE,nullptr);

    if(!directoryEvent) {
        throw DirectoryChangesException(static_cast<int>(GetLastError()));
    }

    m_data->m_overlapped.hEvent=directoryEvent;
    events[0] = m_data->m_terminateEvent;
    events[1] = directoryEvent;

    while(!m_stopped.load()) {
        BYTE buffer[BUFFER_LENGTH];
        ::ReadDirectoryChangesW(m_data->m_directoryHandle,
                                             &buffer,
                                             BUFFER_LENGTH,
                                             TRUE,
                                             FILE_NOTIFY_CHANGE_CREATION |
                                             FILE_NOTIFY_CHANGE_LAST_WRITE |
                                             FILE_NOTIFY_CHANGE_SIZE |
                                             FILE_NOTIFY_CHANGE_DIR_NAME |
                                             FILE_NOTIFY_CHANGE_FILE_NAME,
                                             nullptr,
                                             &m_data->m_overlapped,
                                             nullptr);

        idWaitResult = WaitForMultipleObjects(EventsSize, events, FALSE, Timeout);

        switch (idWaitResult) {
        case WAIT_OBJECT_0: { //terminate event
            m_data->m_directoryHandle = nullptr;
            ::ResetEvent(m_data->m_terminateEvent);
            break;
        }
        case WAIT_OBJECT_0 + 1: { //directory changes event
            auto errorCode = ::GetLastError();
            OnDirectoryChanged(errorCode, buffer, &m_data->m_overlapped, directory, m_data->m_watchedDirectories, sent);
            break;
        }
        default:
            break;
        }
    }
}

void OnDirectoryChanged(DWORD errorCode,
                        BYTE* buffer,
                        LPOVERLAPPED overlapped,
                        const std::string& directory,
                        std::vector<std::string>& directories,
                        send_t& sent) {
    if(errorCode == ERROR_OPERATION_ABORTED) {
        throw DirectoryChangesException(static_cast<int>(errorCode));
    }

    auto transferedBytes = overlapped->InternalHigh;

    //something goes wrong
    if(!transferedBytes || transferedBytes < sizeof(FILE_NOTIFY_INFORMATION)) {
        return;
    }

    BYTE* bytes = buffer;
    while(true) {
        auto notifyInformation = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(bytes);

        auto size = notifyInformation->FileNameLength / 2;
        auto filename = utils::ReplaceAll(utils::ws2s(std::wstring(notifyInformation->FileName, size)), "\\", 1, "/", 1);

        DirectoryChangesType type = DirectoryChangesType::Unknown;

        bool needSend {true};

        switch(notifyInformation->Action) {
            case FILE_ACTION_ADDED: type = DirectoryChangesType::Added; break;
            case FILE_ACTION_REMOVED: type = DirectoryChangesType::Removed; break;
            case FILE_ACTION_MODIFIED: type = DirectoryChangesType::Modified; break;
            case FILE_ACTION_RENAMED_OLD_NAME: type = DirectoryChangesType::OldRenamed; break;
            case FILE_ACTION_RENAMED_NEW_NAME: type = DirectoryChangesType::NewRenamed; break;
            default: needSend = false; break;
        }

        if(needSend) {
            auto path = directory + filename;
            bool isDirectory = utils::PathExists(utils::s2ws(path)) && utils::IsDirectory(utils::s2ws(path));

            if(type == DirectoryChangesType::Added || type == DirectoryChangesType::NewRenamed) {
                if(isDirectory) {
                    auto p = path;
                    if(p.back() != '/') {
                        p += '/';
                    }
                    directories.push_back(p);
                 }
            } else if(type == DirectoryChangesType::Removed || type == DirectoryChangesType::OldRenamed) {
                auto p = path;
                if(p.back() != '/') {
                    p += '/';
                }
                auto found = std::find_if(std::begin(directories), std::end(directories), [&p](const std::string& dir){
                    return p == dir;
                });

                if(found != std::end(directories)) {
                    isDirectory = true;
                    directories.erase(found);
                }
            }
            DirectoryChangesHandler handler {std::move(path), std::move(type), isDirectory};
            auto fInfo = std::make_shared<FileInfo>(handler.handle());
            if(!fInfo->isDirectory ||
               (fInfo->isDirectory && fInfo->type != DirectoryChangesType::Modified)) {
                sent.raise(std::move(fInfo));
            }
        }

        if(!notifyInformation->NextEntryOffset) {
            break;
        }

        bytes += notifyInformation->NextEntryOffset;
    }
}
}
