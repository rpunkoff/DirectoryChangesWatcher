#ifndef DIRECTORY_CONTENT_READER_H
#define DIRECTORY_CONTENT_READER_H

#include <memory>
#include <vector>
#include <string>

using std::string;
using std::vector;
using std::shared_ptr;

namespace api {
class FileInfo;

class DirectoryContentReader final {
public:
	/**
	 * @brief Read Чтение всех файлов в директории
	 * @param directory - путь к директории для чтения
	 * @param relative - относительный путь
	 * @return массив файлов и поддиректорий
	 */
    static vector<shared_ptr<FileInfo>> read(const string& directory,
                                             const string& relative);

private:
    static void internalRead(const string& directory,
                             const string& relative,
                             vector<shared_ptr<FileInfo>>& filesInfo);

};
}

#endif //DIRECTORY_CONTENT_READER_H
