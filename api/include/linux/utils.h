#ifndef UTILS_NIX_H
#define UTILS_NIX_H

#include <string>

#include <sys/stat.h>

namespace utils {
///**
// * @brief FileSize - получение размера файла
// * @param path - путь до файла
// * @return размер файла
// */
//uint64_t FileSize(const std::string& path);

bool IsDirectory(const struct stat& filestat);
int CreateStat(const std::string& path, struct stat& filestat);
}



#endif // UTILS_NIX_H
