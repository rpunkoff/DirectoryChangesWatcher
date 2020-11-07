#ifndef FILES_MODEL_H
#define FILES_MODEL_H

#include <vector>
#include <optional>
#include <unordered_map>

#include "fileinfowrapper.h"

#include <QAbstractTableModel>

class FilesModel : public QAbstractTableModel {
    Q_OBJECT

public:
    FilesModel(QObject *parent = nullptr);
    ~FilesModel() override;

    enum ItemRoles {
        NameRole = Qt::UserRole + 1,
        DateRole = NameRole + 1,
        SizeRole = DateRole + 1,
        DirectoryRole = SizeRole + 1
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    size_t count() const;

    Q_INVOKABLE void clear();

public slots:
    void handle(FileInfoWrapper* fInfo);

signals:
    void countChanged(int count);

private:
    struct InternalData {
        InternalData() = default;
        ~InternalData() = default;

        void add(FileInfoWrapper* fInfo);
        void remove(FileInfoWrapper* fInfo);
        std::optional<size_t> modify(FileInfoWrapper *fInfo);
        void moveFrom(FileInfoWrapper* fInfo);
        std::optional<size_t> moveTo(FileInfoWrapper* fInfo);
        void clear();
        std::optional<size_t> distance(FileInfoWrapper* fInfo) const;
        void updateIndicies();

        std::vector<FileInfoWrapper*> data;
        using data_iterator = std::vector<FileInfoWrapper*>::iterator;
        using const_data_iterator = std::vector<FileInfoWrapper*>::const_iterator;
        std::unordered_map<std::reference_wrapper<std::string>,
                           size_t,
                           std::hash<std::string>,
                           std::equal_to<std::string>> indices;
        FileInfoWrapper* dataToRename{nullptr};
    };

    InternalData m_data;

    void handleAddEvent(FileInfoWrapper* fInfo);
    void handleRemoveEvent(FileInfoWrapper* fInfo);
    void handleModifyEvent(FileInfoWrapper* fInfo);
    void handleOldRenameEvent(FileInfoWrapper* fInfo);
    void handleNewRenameEvent(FileInfoWrapper* fInfo);

};

Q_DECLARE_METATYPE(FilesModel *)

#endif
