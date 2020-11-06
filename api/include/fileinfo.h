#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <string>

#include "directorychangestype.h"

using std::string;

namespace api {
/**
 * @brief The FileInfo struct
 * Информация о файле и изменениях в нем
 */
struct FileInfo {
	bool isDirectory {false};                                  //файл или директория
	DirectoryChangesType type {DirectoryChangesType::Unknown}; //тип изменения
	uint64_t size {0};                                         //размер файла
	string fileName {};                                        //имя файла
	string modificationDate {};                                //дата модификации
};
}

#endif //FILE_INFO_H
