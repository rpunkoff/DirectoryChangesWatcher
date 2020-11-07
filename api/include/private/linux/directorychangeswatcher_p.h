#ifndef DIRECTORY_CHANGES_WATCHER_P_H
#define DIRECTORY_CHANGES_WATCHER_P_H

#include <string>
#include <unordered_map>

#include <sys/inotify.h>
#include <sys/epoll.h>

#define MAX_EVENTS       4096

#define MAX_EPOLL_EVENTS 1
#define EVENT_SIZE       (sizeof (inotify_event))

namespace api {
class DirectoryChangesWatcher;

struct DirectoryChangesWatcherPlatformData {
    DirectoryChangesWatcherPlatformData() = default;
    ~DirectoryChangesWatcherPlatformData() = default;

    std::unordered_map<int, std::string> m_watchedDirectories;

    //TODO maybe move this struct to watcher.cpp
    struct data_t {
        int m_stopFds[2]{};
        int m_epollFd{};
        int m_inotifyFd{};
        struct epoll_event m_inotifyEvent;
        struct epoll_event m_stopEvent;
        struct epoll_event m_events[MAX_EPOLL_EVENTS];
    };

    data_t data;
};

static const uint32_t EventMask{IN_DELETE |
                                IN_MODIFY |
                                IN_CREATE |
                                IN_MOVED_FROM |
                                IN_MOVED_TO};

static const int PipeReadIndex {0};
static const int PipeWriteIndex {1};

}

#endif //DIRECTORY_CHANGES_WATCHER_P_H
