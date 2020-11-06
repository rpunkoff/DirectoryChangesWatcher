#include "../include/directorycontentreader.h"

#include "../include/utils.h"

namespace api {
vector<shared_ptr<FileInfo>> DirectoryContentReader::read(const string& directory, const string& relative) {
    UNUSED(relative)

    vector<shared_ptr<FileInfo>> filesInfo;
    DirectoryContentReader::internalRead(directory.back() == '/' ? directory : directory + "/", relative, filesInfo);

    return filesInfo;
}
}
