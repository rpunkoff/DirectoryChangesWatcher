#include "../include/directorychangeswatcher.h"

#include <directorychangeswatcher_p.h>

namespace api {
DirectoryChangesWatcher::DirectoryChangesWatcher(const std::string& directory) :
    m_directory(directory){
    createData();
}

DirectoryChangesWatcher::~DirectoryChangesWatcher() noexcept {
    stop();
}

bool DirectoryChangesWatcher::start() noexcept {
    if(!isStopped()) {
        return false;
    }

    return internalStart();
}

void DirectoryChangesWatcher::stop() noexcept {
    internalStop();
}

bool DirectoryChangesWatcher::isStopped() const {
    return m_stopped.load();
}

const std::string& DirectoryChangesWatcher::directory() const {
    return m_directory;
}
}
