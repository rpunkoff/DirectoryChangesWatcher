#include "../../include/filerenamer.h"

#include "../../include/utils.h"

#include <cstdio>

namespace api {
bool Rename(const std::string& from, const std::string& to) {
    return rename(from.c_str(), to.c_str()) == 0;
}

bool Rename(const std::wstring& from, const std::wstring& to) {
    auto fromString = utils::ws2s(from);
    auto toString = utils::ws2s(to);
    return Rename(fromString, toString);
}
}
