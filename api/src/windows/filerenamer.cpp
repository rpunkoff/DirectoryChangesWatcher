#include "../../include/filerenamer.h"
#include "../../include/utils.h"

#include <windows.h>

namespace api {
bool Rename(const std::wstring& from, const std::wstring& to) {
    //запуск переименования файла
	return MoveFileW(from.c_str(), to.c_str()) != 0;
}
}
