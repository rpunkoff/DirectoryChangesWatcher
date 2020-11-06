#include "../../include/linux/utils.h"

namespace utils {
bool IsDirectory(const struct stat& filestat) {
    return S_ISDIR(filestat.st_mode);
}

int CreateStat(const std::string& path, struct stat& filestat) {
    auto statResult = stat(path.c_str(), &filestat);

    return statResult != 0 ? errno : 0;
}
}
