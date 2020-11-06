#include "../../include/directorychangeshandler.h"

#include "../../include/windows/utils.h"
#include "../../include/utils.h"

namespace api {
/**
 * @brief OnChanged - обработка измененич файла
 * @param fInfo - структура, содержащая информацию о файле
 */
void DirectoryChangesHandler::OnChanged(FileInfo& fInfo) {
    WIN32_FIND_DATAW fData {};
    //поиск файла на файловой системе
    utils::FilesFinder finder(utils::s2ws(fInfo.fileName), fData);

    if(finder.isValid()) {
        //проверка файли - директория или нет
        if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            fInfo.isDirectory = true;
        }

        //получение информации о файле
        auto fileTime = fData.ftLastWriteTime;
        fInfo.modificationDate = utils::FileTimeToString(fileTime);
        fInfo.size = utils::FileSize(fData.nFileSizeHigh, fData.nFileSizeLow);
    }
}


}
