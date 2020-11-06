#ifndef DIRECTORY_CHANGES_HANDLER_H
#define DIRECTORY_CHANGES_HANDLER_H

#include "fileinfo.h"

#include <optional>

namespace api {
/**
 * @brief The DirectoryChangesHandler class
 * Обработчик сигналов операционной системы об изменении в директории
 */
class DirectoryChangesHandler final {
public:
    /**
     * @brief DirectoryChangesHandler ctor
     * @param fileName - имя файла
     * @param type - тип события
     */
    DirectoryChangesHandler(const std::string &fileName,
                            DirectoryChangesType type);
    DirectoryChangesHandler(const std::string &fileName,
                            DirectoryChangesType type,
                            bool isDirectory);

    DirectoryChangesHandler(const DirectoryChangesHandler&) = default;
    DirectoryChangesHandler(DirectoryChangesHandler&& handler);

    DirectoryChangesHandler& operator=(const DirectoryChangesHandler&) = default;
    DirectoryChangesHandler& operator=(DirectoryChangesHandler&& handler);

    ~DirectoryChangesHandler() noexcept = default;

    /**
     * @brief handle обработка файла
     * @return структуру, содержащую информацию о файле и о его изменении
     */
    FileInfo handle() noexcept;

private:
    std::string m_fileName;        //имя файла
    DirectoryChangesType m_type;   //тип изменения
    std::optional<bool> m_isDirectory{};

    void OnChanged(FileInfo& fInfo);
};
}

#endif //DIRECTORY_CHANGES_HANDLER_H
