#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <string>

#include "directorychangestype.h"

using std::string;

namespace api {
/**
 * @brief The FileInfo struct
 * An information about a changed file
 */
struct FileInfo {
    bool isDirectory {false};                                  //directory or file
    DirectoryChangesType type {DirectoryChangesType::Unknown}; //change type
    uint64_t size {0};                                         //file size
    string fileName {};                                        //file name
    string modificationDate {};                                //modification date
};
}

#endif //FILE_INFO_H
