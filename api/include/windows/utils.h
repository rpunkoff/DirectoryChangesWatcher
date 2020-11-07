#ifndef UTILS_WIN_H
#define UTILS_WIN_H

#include <string>

#include <windows.h>

namespace utils {
/**
 * @brief The FilesFinder class
 * Wrapper for recursively file search in directory and it's subdirectories
 */
class FilesFinder final {
public:
    /**
     * @brief FilesFinder - ctor
     * Start FindFirstFile and set handle
     */
    FilesFinder(const std::wstring& filePath, WIN32_FIND_DATAW& fData) noexcept;
    ~FilesFinder();

    bool findNext(WIN32_FIND_DATAW& fData);

    bool isValid();

private:
    HANDLE m_handle {nullptr};
};

std::string FileTimeToString(const FILETIME& ftime );

uint64_t FileSize(DWORD fileSizeHigh, DWORD fileSIzeLow);

bool IsDirectory(const std::wstring& path);
bool IsDirectory(const std::string& path);

bool PathExists(const std::wstring& path);
bool PathExists(const std::string& path);
}

#endif // UTILS_WIN_H
