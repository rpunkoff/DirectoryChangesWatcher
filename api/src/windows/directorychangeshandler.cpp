#include "../../include/directorychangeshandler.h"

#include "../../include/windows/utils.h"
#include "../../include/utils.h"

namespace api {
void DirectoryChangesHandler::OnChanged(FileInfo& fInfo) {
    WIN32_FIND_DATAW fData {};
    utils::FilesFinder finder(utils::s2ws(fInfo.fileName), fData);

    if(finder.isValid()) {
        if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            fInfo.isDirectory = true;
        }

        auto fileTime = fData.ftLastWriteTime;
        fInfo.modificationDate = utils::FileTimeToString(fileTime);
        fInfo.size = utils::FileSize(fData.nFileSizeHigh, fData.nFileSizeLow);
    }
}
}
