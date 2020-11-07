#ifndef UTILS_NIX_H
#define UTILS_NIX_H

#include <string>

#include <sys/stat.h>

namespace utils {
bool IsDirectory(const struct stat& filestat);
int CreateStat(const std::string& path, struct stat& filestat);
}



#endif // UTILS_NIX_H
