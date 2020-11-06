#include "../../include/directorycontentreader.h"
#include "../../include/directorychangesexception.h"
#include "../../include/fileinfo.h"

#include "../../include/windows/utils.h"
#include "../../include/utils.h"

using utils::FilesFinder;
using utils::FileTimeToString;
using utils::FileSize;
using utils::ReplaceAll;

#include <iostream>

namespace api {
/**
 * @brief FillFileInfo заполнение структуры FileInfo из полученной WIN32_FIND_DATAW
 * @param fData структура WIN32_FIND_DATAW
 * @return структура FileInfo
 */
shared_ptr<FileInfo> FillFileInfo(const std::string& directory, const WIN32_FIND_DATAW& fData) {
    shared_ptr<FileInfo> fInfo = std::make_shared<FileInfo>();
    fInfo->isDirectory = (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;

    fInfo->modificationDate = FileTimeToString(fData.ftCreationTime);
    fInfo->size = FileSize(fData.nFileSizeHigh, fData.nFileSizeLow);
    fInfo->type = DirectoryChangesType::Added;
    //замена бэкслешей на нормальные слеши
    fInfo->fileName = directory;
    std::string filename = utils::ws2s(fData.cFileName);
    fInfo->fileName.reserve(directory.size() + filename.size() + 2);
    if(directory.back() != '/') {
        fInfo->fileName += '/';
    }

    fInfo->fileName += utils::ReplaceAll(filename, "\\", 1, "/", 1);

    if(fInfo->isDirectory && fInfo->fileName.back() != '/') {
        fInfo->fileName += '/';
    }

    return fInfo;
}

using read_t = utils::Event<const string&,
                            const string&,
                            vector<shared_ptr<FileInfo>>&>;

inline void InsertFile(const std::string& directory,
                       const WIN32_FIND_DATAW& fData,
                       std::vector<shared_ptr<FileInfo>>& filesInfo,
                       read_t fn) {
    //не нужно в список выводить "." и ".."
    if(wcscmp(fData.cFileName, L".") != 0 &&
       wcscmp(fData.cFileName, L"..") != 0) {

        auto found = std::find_if(std::rbegin(filesInfo), std::rend(filesInfo), [&f = directory](const shared_ptr<FileInfo>& item){
                return f == item->fileName;
            });

        if(found != std::rend(filesInfo)) {
            filesInfo.erase((found + 1).base());
        }

        filesInfo.push_back(FillFileInfo(directory, fData));
        //если найден файл
        if((fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
            auto temp = ReplaceAll( utils::ws2s(fData.cFileName), "\\", 1, "/", 1) + "/";
            //рекурсивный запуск чтения в поддиректории
            fn.raise(directory, temp, filesInfo);
        }
    }
}

/**
 * @brief InternalRead рекурсивное чтение директории
 * @param directory директория для чтения
 * @param relative относительный путь
 * @return массив структур FileInfo
 */
void DirectoryContentReader::internalRead(const string& directory,
                                          const string& relative,
                                          vector<shared_ptr<FileInfo>>& filesInfo) {
    WIN32_FIND_DATAW fData {};

    string fullPath(directory);
    fullPath.reserve(directory.size() + relative.size() + 1);
    if(directory.back() != '/') {
       fullPath += '/';
    }

    fullPath += relative;

    //поиск первого вхождения
    FilesFinder finder(utils::s2ws(fullPath + "/*"), fData);

    //проверка: получен ли HANDLE
    if(!finder.isValid()){
       throw DirectoryChangesException(static_cast<int>(GetLastError()));
    }

    read_t event;
    event.attachFunction(DirectoryContentReader::internalRead);

    //цикл чтения всех вхождений
    while (true) {
        InsertFile(fullPath, fData, filesInfo, event);
        auto result = finder.findNext(fData);
        if(!result) {
            break;
        }
    }
}
}



