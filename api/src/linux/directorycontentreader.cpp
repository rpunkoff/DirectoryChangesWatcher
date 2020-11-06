#include "../../include/directorycontentreader.h"
#include "../../include/directorychangesexception.h"
#include "../../include/directorychangestype.h"
#include "../../include/fileinfo.h"

#include "../../include/utils.h"
#include "../../include/linux/utils.h"

#include <dirent.h>
#include <time.h>
#include <iomanip>

#include <sstream>

#include <string.h>
#include <errno.h>

namespace api {
/**
 * @brief FillFileInfo заполнение структуры FileInfo из полученной dirent
 * @param fData структура WIN32_FIND_DATAW
 * @return структура FileInfo
 */
FileInfo FillFileInfo(const string filename, const struct stat& filestat, bool isDirectory) {
    FileInfo fInfo;

    fInfo.isDirectory = isDirectory;
    struct tm* _tm{};
    _tm = gmtime(&(filestat.st_mtime));
    std::ostringstream oss;
    oss << std::put_time(_tm, "%d-%m-%Y %H-%M-%S");
    fInfo.modificationDate = oss.str();
    fInfo.size =  static_cast<uint64_t>(filestat.st_size);
    fInfo.type = DirectoryChangesType::Added;
    fInfo.fileName = filename;

    return fInfo;
}

/**
 * @brief InternalRead рекурсивное чтение директории
 * @param directory директория для чтения
 * @param relative относительный путь
 * @param filesInfo массив структур FileInfo
*/
void DirectoryContentReader::internalRead(const string& directory, const string& relative, vector<shared_ptr<FileInfo>>& filesInfo) {
    DIR* d{};
    dirent* dir{};

    string fullPath(directory);
    fullPath.reserve(directory.size() + relative.size() + 1);
    fullPath += relative;

    d = opendir(fullPath.c_str());

    if(!d) {
        throw DirectoryChangesException(errno);
    }

    while (true) {
        errno = {};
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

        string newFullPath;
        newFullPath.reserve(fullPath.size() + strnlen(dir->d_name, 256));
        newFullPath += fullPath;
        if(fullPath.back() != '/') {
            newFullPath += "/";
        }
        newFullPath += dir->d_name;

        struct stat filestat;
        auto statResult = utils::CreateStat(newFullPath, filestat);

        if(statResult) {
            throw DirectoryChangesException(statResult);
        }

        bool isDirectory = utils::IsDirectory(filestat);
        auto fileInfo = FillFileInfo(newFullPath, filestat, isDirectory);

        auto found = std::find_if(std::rbegin(filesInfo), std::rend(filesInfo), [&f = fullPath](const shared_ptr<FileInfo>& item){
            return f == item->fileName;
        });

        if(found != std::rend(filesInfo)) {
            filesInfo.erase((found + 1).base());
        }

        filesInfo.push_back(std::make_shared<FileInfo>(std::move(fileInfo)));

        if(isDirectory) {
            if(fullPath.back() != '/') {
                fullPath += '/';
            }
            internalRead(fullPath, dir->d_name, filesInfo);
        }
    }
}
}
