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
}
