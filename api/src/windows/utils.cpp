#include "../../include/windows/utils.h"

#include "../../include/utils.h"

#include <shlwapi.h>

namespace utils {
FilesFinder::FilesFinder(const std::wstring& filePath, WIN32_FIND_DATAW& fData) noexcept {
    m_handle = ::FindFirstFileW(filePath.c_str(), &fData);
}

FilesFinder::~FilesFinder() {
    if(isValid()) {
        ::FindClose(m_handle);
    }
}

bool FilesFinder::findNext(WIN32_FIND_DATAW& fData) {
    return FindNextFileW(m_handle, &fData) != FALSE;
}

bool FilesFinder::isValid() {
    return m_handle && m_handle != INVALID_HANDLE_VALUE;
}

std::string FileTimeToString(const FILETIME& ftime ) // ISO format, time zone designator Z == zero (UTC)
{
    SYSTEMTIME utc ;
    ::FileTimeToSystemTime( std::addressof(ftime), std::addressof(utc) );

    std::ostringstream stm ;
    const auto w2 = std::setw(2) ;
    stm << std::setfill('0') <<
           w2 << utc.wDay << '.' <<
           w2 << utc.wMonth << '.' <<
           std::setw(4) << utc.wYear << ' ' <<
           w2 << utc.wHour<< ':' <<
           w2 << utc.wMinute << ':' <<
           w2 << utc.wSecond;

    return stm.str();
}

uint64_t FileSize(DWORD fileSizeHigh, DWORD fileSIzeLow) {
    uint64_t fileSize = fileSizeHigh;
    fileSize <<= sizeof(fileSizeHigh) * 8;
    fileSize |= fileSIzeLow;

    return fileSize;
}

bool IsDirectory(const std::wstring& path) {
    auto attributes = GetFileAttributesW(path.c_str());

    if(attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    return (attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

bool IsDirectory(const std::string& path) {
    auto attributes = GetFileAttributesA(path.c_str());

    if(attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    return (attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

bool PathExists(const std::wstring& path) {
    auto attributes = GetFileAttributesW(path.c_str());

    return attributes != INVALID_FILE_ATTRIBUTES;
}

bool PathExists(const std::string& path) {
    auto attributes = GetFileAttributesA(path.c_str());

    return attributes != INVALID_FILE_ATTRIBUTES;
}
}
