#include "../../include/directorychangeswatcher.h"
#include "../../include/directorychangesexception.h"
#include "../../include/directorychangeshandler.h"

#include "../../include/linux/utils.h"

#include <directorychangeswatcher_p.h>

#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <type_traits> //for add_pointer

#include <iostream>

namespace api {
void DirectoryChangesWatcher::createData() {
    m_data = std::make_unique<DirectoryChangesWatcherPlatformData>();
}

void DirectoryChangesWatcher::internalStop() {
    if(!isStopped()) {
        m_stopped.store(true);
        uint8_t buf[1]{0};
        auto result = write(m_data->data.m_stopFds[PipeWriteIndex], buf, 1);
        UNUSED(result);
    }
}

bool DirectoryChangesWatcher::internalStart() {
    return !(m_stopped = false);
}

bool DirectoryChangesWatcher::isValid() const {
    std::lock_guard l(m_mutex);
    return m_data && m_data->data.m_epollFd &&
            m_data->data.m_inotifyFd;
}

void ThrowIfErrorOccured(int errorCode) {
    if(errorCode == -1) {
        throw DirectoryChangesException(errno);
    }
}

void InitWatching(DirectoryChangesWatcherPlatformData::data_t& data) {
    ThrowIfErrorOccured(pipe2(data.m_stopFds, O_NONBLOCK));

    data.m_inotifyFd = inotify_init1(IN_NONBLOCK);
    ThrowIfErrorOccured(data.m_inotifyFd);

    data.m_epollFd =  epoll_create1(0);
    ThrowIfErrorOccured(data.m_epollFd);

    data.m_inotifyEvent.events = EPOLLIN | EPOLLET;
    data.m_inotifyEvent.data.fd =  data.m_inotifyFd;
    ThrowIfErrorOccured(epoll_ctl(data.m_epollFd, EPOLL_CTL_ADD, data.m_inotifyFd, &data.m_inotifyEvent));

    data.m_stopEvent.events = EPOLLIN | EPOLLET;
    data.m_stopEvent.data.fd = data.m_stopFds[PipeReadIndex];
    ThrowIfErrorOccured(epoll_ctl(data.m_epollFd, EPOLL_CTL_ADD, data.m_stopFds[PipeReadIndex], &data.m_stopEvent));
}

/**
 * @brief WatchDirectories - start the recursyvely watching
 * @param directory - path
 * @param data - platform dependent internal data
 * @param directoriesToWatch - list of directories for watching
 */
void WatchDirectories(const std::string& directory,
                      const DirectoryChangesWatcherPlatformData::data_t& data,
                      std::unordered_map<int, std::string>& directoriesToWatch ) {
    DIR* d{};
    dirent* dir{};

    //try to open dir
    d = opendir(directory.c_str());

    if(!d) {
        throw DirectoryChangesException(errno);
    }

    //add dir to watching
    int watchDescriptor = inotify_add_watch(data.m_inotifyFd, directory.c_str(), EventMask);

    ThrowIfErrorOccured(watchDescriptor);

    directoriesToWatch[watchDescriptor] = directory.back() == '/' ? directory : directory + "/";

    while (true) {
        errno = {};
        //read the current dir
        dir = readdir(d);
        if(!dir) {
            if(!errno) {
                break;
            } else {
                throw DirectoryChangesException(errno);
            }
        }

        if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
            continue;
        }

        //create new path
        std::string newFullPath;
        newFullPath.reserve(directory.size() + strnlen(dir->d_name, 256));
        newFullPath += directory;
        if(directory.back() != '/') {
            newFullPath += "/";
        }
        newFullPath += dir->d_name;

        struct stat filestat;
        //create stat
        auto statResult = utils::CreateStat(newFullPath, filestat);

        ThrowIfErrorOccured(statResult);

        bool isDirectory = utils::IsDirectory(filestat);

        if(isDirectory) {
            //watch subdirectory
             WatchDirectories(newFullPath, data, directoriesToWatch);
        }
    }
}

/**
 * @brief CloseWatching - close watching
 * @param data - platform dependent internal data
 */
void CloseWatching(DirectoryChangesWatcherPlatformData::data_t& data) {
    if(data.m_inotifyFd) {
        epoll_ctl(data.m_epollFd, EPOLL_CTL_DEL, data.m_inotifyFd, 0);
    }

    if(data.m_stopFds[PipeReadIndex]) {
        epoll_ctl(data.m_epollFd, EPOLL_CTL_DEL, data.m_stopFds[PipeReadIndex], 0);
    }

    if(data.m_inotifyFd) {
        ThrowIfErrorOccured(close(data.m_inotifyFd));
    }

    if(data.m_epollFd) {
        ThrowIfErrorOccured(close(data.m_epollFd));
    }

    close(data.m_stopFds[PipeReadIndex]);
    close(data.m_stopFds[PipeWriteIndex]);
}

const size_t EVENT_BUFFER_SIZE = MAX_EVENTS * (EVENT_SIZE + 16);

/**
 * @brief ReadEvents - read events from raw data buffer
 * @param data - platform dependent internal data
 * @param watchedDirectories - list of watching directories
 * @param eventsData - raw data buffer
 * @param size - size of buffer
 * @param sent - callback to sent an event about some changes
 */
void ReadEvents(const DirectoryChangesWatcherPlatformData::data_t& data,
                std::unordered_map<int, std::string>& watchedDirectories,
                uint8_t* eventsData,
                size_t size,
                send_t& sent) {
    for(size_t index = 0; index < size;) {
        inotify_event* event = (struct inotify_event *)&eventsData[index];
        index +=EVENT_SIZE;

        if((event->mask & IN_IGNORED) != 0) {
            continue;
        }

        //check for event-> wd exists in the watching directories list
        auto iter = watchedDirectories.find(event->wd);

        if(iter == std::end(watchedDirectories)) {
            throw DirectoryChangesException(ENODATA);
        }

        auto path = iter->second + reinterpret_cast<char*>(&eventsData[index]);

        DirectoryChangesType type = DirectoryChangesType::Unknown;

        bool needSend {true};
        bool isDirectory {false};

        //determine event is dir or file
        if((event->mask & IN_ISDIR) != 0) {
            event->mask = event->mask & ~IN_ISDIR;
            isDirectory = true;
        }

        //get event type
        switch(event->mask) {
            case IN_DELETE:     type = DirectoryChangesType::Removed;    break;
            case IN_MODIFY:     type = DirectoryChangesType::Modified;   break;
            case IN_CREATE:     type = DirectoryChangesType::Added;      break;
            case IN_MOVED_FROM: type = DirectoryChangesType::OldRenamed; break;
            case IN_MOVED_TO:   type = DirectoryChangesType::NewRenamed; break;
            default: needSend = false; break;
        }

        if(isDirectory) {
            if(type == DirectoryChangesType::Added || type == DirectoryChangesType::NewRenamed) {
                //if some directory was added to fs then this directory will add into directories list
                int watchDescriptor = inotify_add_watch(data.m_inotifyFd, path.c_str(), EventMask);
                ThrowIfErrorOccured(watchDescriptor);
                watchedDirectories[watchDescriptor] = path.back() == '/' ? path : path + "/";
                if(path.back() == '/') {
                    path.erase(path.size() - 1);
                }
            } else if(type == DirectoryChangesType::Removed || type == DirectoryChangesType::OldRenamed) {
                //if some directory was removed from fs then this shouldn't be on the watchlist
                int result = inotify_rm_watch(data.m_inotifyFd, iter->first);
                ThrowIfErrorOccured(result);
                iter = watchedDirectories.erase(iter);
            }
        }

        if(needSend) {
            DirectoryChangesHandler handler {std::move(path), std::move(type), std::move(isDirectory)};
            //send the callback with the event
            sent.raise(std::make_shared<FileInfo>(handler.handle()));
        }

        index += event->len;
    }
}

/**
 * @brief HandleEvents - handle events about some changes in watching directories
 * @param watchedDirectories - list of watching directories
 * @param data - platform dependent internal data
 * @param sent - callback to sent an event about some changes
 */
void HandleEvents(std::unordered_map<int, std::string>& watchedDirectories,
                  DirectoryChangesWatcherPlatformData::data_t& data,
                  send_t& sent) {
    static uint8_t eventBuffer[EVENT_BUFFER_SIZE] {};
    auto timeout = -1;
    //wait for an event rises
    auto readyFdsCount = epoll_wait(data.m_epollFd, data.m_events, MAX_EPOLL_EVENTS, timeout);

    if(readyFdsCount == -1) {
        return;
    }

    for(auto index = 0; index < readyFdsCount; ++index) {
        if (data.m_events[index].data.fd == data.m_stopFds[PipeReadIndex]) {
            break;
        }

        //read data into "eventBuffer"
        auto length = read(data.m_events[index].data.fd, eventBuffer, EVENT_BUFFER_SIZE);
        if(length == -1) {
            if(errno == EINTR) {
                break;
            }
        } else {
            //read events
            ReadEvents(data, watchedDirectories, eventBuffer, length, sent);
        }
    }
}

namespace {
using close_watching_t = std::add_pointer<void(DirectoryChangesWatcherPlatformData::data_t& data)>::type;

class CloseWatchingGuard {
public:
    CloseWatchingGuard(close_watching_t fn, DirectoryChangesWatcherPlatformData::data_t& d);
    ~CloseWatchingGuard();

private:
    close_watching_t m_fn;
    DirectoryChangesWatcherPlatformData::data_t& m_data;
};

CloseWatchingGuard::CloseWatchingGuard(close_watching_t fn, DirectoryChangesWatcherPlatformData::data_t& d) :
    m_fn(fn),
    m_data(d){ }

CloseWatchingGuard::~CloseWatchingGuard(){
    m_fn(m_data);
}
}

void DirectoryChangesWatcher::run() {
	createData();
    InitWatching(m_data->data);
    WatchDirectories(directory(), m_data->data, m_data->m_watchedDirectories);

    CloseWatchingGuard guard(&CloseWatching, m_data->data);

    while (true) {
        if (isStopped()) {
          break;
        }

        HandleEvents(m_data->m_watchedDirectories, m_data->data, sent);
    }
}
}
