#ifndef DIRECTORY_CHANGES_WATCHER_H
#define DIRECTORY_CHANGES_WATCHER_H

#include <memory>
#include <atomic>

#include "utils.h"

namespace api {
class FileInfo;

using send_t = utils::Event<std::shared_ptr<FileInfo>>;

struct DirectoryChangesWatcherPlatformData;

/**
 * @brief The DirectoryChangesWatcher class обертка для наблюдения за директориями
 */
class DirectoryChangesWatcher final {
public:
    explicit DirectoryChangesWatcher(const std::string& directory);

    DirectoryChangesWatcher(const DirectoryChangesWatcher&) = delete;
    DirectoryChangesWatcher& operator=(const DirectoryChangesWatcher&) = delete;

    DirectoryChangesWatcher(DirectoryChangesWatcher&&) = delete;
    DirectoryChangesWatcher& operator=(DirectoryChangesWatcher&&) = delete;

    ~DirectoryChangesWatcher() noexcept;

    const std::string& directory() const;

    bool isStopped() const;

    bool start() noexcept;
    void run();
    void stop() noexcept;

    //callback для отправки события об изменении в директории
    send_t sent;

private:
    void createData();
    bool internalStart();
    void internalStop();

    std::atomic_bool m_stopped {true};    //флаг остановки
    std::string m_directory;  //директория наблюдения
    std::unique_ptr<DirectoryChangesWatcherPlatformData> m_data;
};
}

#endif //DIRECTORY_CHANGES_WATCHER_H

