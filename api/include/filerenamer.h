#ifndef FILE_RENAMER_H
#define FILE_RENAMER_H

#include <string>

namespace api {
    /**
     * @brief Rename - rename given file "from" on filesystem to file "to".
     * Paths "from" and "to" should be full paths
     * @param from - old path
     * @param to - new path
     * @return This function will return true if the file "from" was renamed to the file "to", overwise - false
     */
    bool Rename(const std::wstring& from, const std::wstring& to);
}

#endif //FILE_RENAMER_H
