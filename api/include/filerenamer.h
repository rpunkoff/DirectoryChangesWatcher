#ifndef FILE_RENAMER_H
#define FILE_RENAMER_H

#include <string>

namespace api {
    /**
     * @brief Rename - переименование файла на файловой системе.
     * В параметрах должны быть указаны не просто имена файлов, а полный путь
     * @param from - старое имя
     * @param to - новое имя
     * @return  true - если файл был переименован, в противном случае - false
     */
    bool Rename(const std::wstring& from, const std::wstring& to);
}

#endif //FILE_RENAMER_H
