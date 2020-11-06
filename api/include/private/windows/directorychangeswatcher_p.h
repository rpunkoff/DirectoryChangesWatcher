#ifndef DIRECTORY_CHANGES_WATCHER_P_H
#define DIRECTORY_CHANGES_WATCHER_P_H

#include <windows.h>
#include <vector>
#include <string>

namespace api {
const size_t BUFFER_LENGTH = 100000 * sizeof(FILE_NOTIFY_INFORMATION);

class DirectoryChangesWatcher;

struct DirectoryChangesWatcherPlatformData {
    DirectoryChangesWatcherPlatformData() = default;
    ~DirectoryChangesWatcherPlatformData() = default;

    std::vector<std::string> m_watchedDirectories;
    HANDLE m_directoryHandle {nullptr};
    HANDLE m_terminateEvent {nullptr};
    OVERLAPPED m_overlapped {};
};
}

#endif //DIRECTORY_CHANGES_WATCHER_P_H
