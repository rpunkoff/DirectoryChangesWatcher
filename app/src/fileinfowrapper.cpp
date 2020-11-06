#include "../include/fileinfowrapper.h"

#include <fileinfo.h>

FileInfoWrapper::FileInfoWrapper(std::shared_ptr<api::FileInfo> fInfo, QObject * parent) :
    QObject (parent),
    m_fInfo(std::move(fInfo)) { }

const std::shared_ptr<api::FileInfo>& FileInfoWrapper::fileInfo() const {
    return m_fInfo;
}

void FileInfoWrapper::setFileInfo(const std::shared_ptr<api::FileInfo>& fInfo) {
    m_fInfo = fInfo;
}
