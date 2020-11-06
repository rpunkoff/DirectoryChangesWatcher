#include "../../include/directorychangeshandler.h"

#include "../../include/linux/utils.h"

#include <cstring>

namespace api {
/**
 * @brief OnChanged - обработка измененич файла
 * @param fInfo - структура, содержащая информацию о файле
 */
void DirectoryChangesHandler::OnChanged(FileInfo& fInfo) {
    struct stat filestat;
    auto path = fInfo.fileName.c_str();
    utils::CreateStat(path, filestat);
    fInfo.isDirectory = m_isDirectory == std::nullopt ? utils::IsDirectory(filestat) : m_isDirectory.value();
    char time[20]{};
    strftime(time, 20, "%d.%m.%Y %H:%M:%S", localtime(&filestat.st_mtime));
    fInfo.modificationDate = std::string(time, strlen(time));
    fInfo.size = static_cast<unsigned long>(filestat.st_size);
}

// FileInfo DirectoryChangesHandler::handle() noexcept {
    // //создание структуры с именем файла и типом изменения
    // FileInfo fInfo{ m_isDirectory == std::nullopt ? false :  m_isDirectory.value(), m_type, 0, m_fileName, ""};

    // switch (m_type) {
    // case DirectoryChangesType::Removed :
    // case DirectoryChangesType::OldRenamed: return fInfo;
    // case DirectoryChangesType::Added:
    // case DirectoryChangesType::Modified:
    // case DirectoryChangesType::NewRenamed:
        // //обработка изменения
        // OnChanged(fInfo);
        // return fInfo;
    // default: break;
    // }

    // return FileInfo{};
// }
}
