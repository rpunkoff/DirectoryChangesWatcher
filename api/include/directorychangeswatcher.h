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
 * @brief The DirectoryChangesWatcher class
 * This class watches for all changes in a given directory including subdirectories
 * This class doesn't create any threads and work in single thread mode
 * For this class usage see the tests
 */
class DirectoryChangesWatcher final {
public:
    explicit DirectoryChangesWatcher(const std::string& directory);

    DirectoryChangesWatcher(const DirectoryChangesWatcher&) = delete;
    DirectoryChangesWatcher& operator=(const DirectoryChangesWatcher&) = delete;

    DirectoryChangesWatcher(DirectoryChangesWatcher&&) = delete;
    DirectoryChangesWatcher& operator=(DirectoryChangesWatcher&&) = delete;

    ~DirectoryChangesWatcher() noexcept;

    /**
     * @brief directory
     * @return Get the given directory for watching
     */
    const std::string& directory() const;

    /**
     * @brief isStopped - Check for watching stopped or not
     * @return if the watching is stopped this method returns true, overwise - false
     */
    bool isStopped() const;


    /**
     * @brief start - check for watching for directory stopped or not and if it is stopped then this method sets the flag stopped into the "false" state
     * @return if watching was not stopped then this method returns "false", overwise - true
     */
    bool start() noexcept;

    /**
     * @brief run - Implementation of watching for directory changes
     * This method should execute in another thread
     */
    void run();

    /**
     * @brief stop - Stop the watching for directory
     */
    void stop() noexcept;

    //callback for sending an event about some changes in the watching directory
    send_t sent;

private:
    void createData();
    bool internalStart();
    void internalStop();

    std::atomic_bool m_stopped {true};
    std::string m_directory;
    std::unique_ptr<DirectoryChangesWatcherPlatformData> m_data;
};
}

#endif //DIRECTORY_CHANGES_WATCHER_H

