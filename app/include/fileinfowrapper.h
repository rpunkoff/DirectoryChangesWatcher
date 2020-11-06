#ifndef QT_FILE_INFO_WRAPPER_H
#define QT_FILE_INFO_WRAPPER_H

#include <QObject>

#include <memory>

#include <fileinfo.h>

/**
 * @brief The FileInfo class
 * Хранилище информации о файле, используется в моделе
 */
class FileInfoWrapper final : public QObject {
	Q_OBJECT
public:
    FileInfoWrapper(std::shared_ptr<api::FileInfo> fInfo, QObject * parent = nullptr);
    ~FileInfoWrapper() override = default;

    const std::shared_ptr<api::FileInfo>& fileInfo() const;

    void setFileInfo(const std::shared_ptr<api::FileInfo> &fileInfo);
private:
    std::shared_ptr<api::FileInfo> m_fInfo;
};

Q_DECLARE_METATYPE(FileInfoWrapper *)

#endif //QT_FILE_INFO_WRAPPER_H
