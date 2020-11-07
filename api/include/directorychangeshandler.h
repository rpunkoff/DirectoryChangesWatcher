#ifndef DIRECTORY_CHANGES_HANDLER_H
#define DIRECTORY_CHANGES_HANDLER_H

#include "fileinfo.h"

#include <optional>

namespace api {
/**
 * @brief The DirectoryChangesHandler class
 * File changes event handler
 */
class DirectoryChangesHandler final {
public:
    /**
     * @brief DirectoryChangesHandler ctor
     * @param fileName - file name
     * @param type - changes type
     */
    DirectoryChangesHandler(const std::string &fileName,
                            DirectoryChangesType type);
    /**
     * @brief DirectoryChangesHandler ctor
     * @param fileName - file name
     * @param type - changes type
     * @param isDirectory - path is directory or not
     */
    DirectoryChangesHandler(const std::string &fileName,
                            DirectoryChangesType type,
                            bool isDirectory);

    DirectoryChangesHandler(const DirectoryChangesHandler&) = default;
    DirectoryChangesHandler(DirectoryChangesHandler&& handler);

    DirectoryChangesHandler& operator=(const DirectoryChangesHandler&) = default;
    DirectoryChangesHandler& operator=(DirectoryChangesHandler&& handler);

    ~DirectoryChangesHandler() noexcept = default;

    /**
     * @brief handle - To handle event
     * @return Some info about changes in the file
     */
    FileInfo handle() noexcept;

private:
    std::string m_fileName;        //file name
    DirectoryChangesType m_type;   //changes type
    std::optional<bool> m_isDirectory{};

    /**
     * @brief OnChanged - handle "add", "modify", "new renamed" events
     * @param fInfo - given file info
     */
    void OnChanged(FileInfo& fInfo);
};
}

#endif //DIRECTORY_CHANGES_HANDLER_H
