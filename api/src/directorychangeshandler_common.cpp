#include "../include/directorychangeshandler.h"

namespace api {
DirectoryChangesHandler::DirectoryChangesHandler(const std::string& fileName,
                                                 DirectoryChangesType type) :
    m_fileName(fileName),
    m_type(type){ }

DirectoryChangesHandler::DirectoryChangesHandler(const std::string &fileName,
                                                 DirectoryChangesType type,
                                                 bool isDirectory) :
    m_fileName(fileName),
    m_type(type),
    m_isDirectory(isDirectory){ }

DirectoryChangesHandler::DirectoryChangesHandler(DirectoryChangesHandler&& handler) {
    *this = std::move(handler);
}

DirectoryChangesHandler& DirectoryChangesHandler::operator=(DirectoryChangesHandler&& handler) {
    if(this == &handler) {
        return *this;
    }

    m_fileName = std::move(handler.m_fileName);
    m_type = std::move(handler.m_type);

    handler.m_type = DirectoryChangesType::Unknown;

    return *this;
}

FileInfo DirectoryChangesHandler::handle() noexcept {
    //создание структуры с именем файла и типом изменения
    FileInfo fInfo{ m_isDirectory == std::nullopt ? false : m_isDirectory.value(), m_type, 0, m_fileName, ""};

    switch (m_type) {
    case DirectoryChangesType::Removed :
    case DirectoryChangesType::OldRenamed: return fInfo;
    case DirectoryChangesType::Added:
    case DirectoryChangesType::Modified:
    case DirectoryChangesType::NewRenamed:
        //обработка изменения
        OnChanged(fInfo);
        return fInfo;
    default: break;
    }

    return FileInfo{};
}
}
