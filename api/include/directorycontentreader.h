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
     * @brief Read - Reading all files in a directory with it's subdirectories
     * @param directory - directory path
     * @return Array of found files adn directories
     */
    static vector<shared_ptr<FileInfo>> read(const string& directory);

private:
    /**
     * Platform dependent implementation
    */
    static void internalRead(const string& directory,
                             const string& relative,
                             vector<shared_ptr<FileInfo>>& filesInfo);

};
}

#endif //DIRECTORY_CONTENT_READER_H
