#include "../../include/filerenamer.h"
#include "../../include/utils.h"

#include <windows.h>

namespace api {
bool Rename(const std::wstring& from, const std::wstring& to) {
    return MoveFileW(from.c_str(), to.c_str()) != 0;
}
}
