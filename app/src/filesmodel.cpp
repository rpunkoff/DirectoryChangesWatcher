#include "../include/filesmodel.h"

FilesModel::FilesModel(QObject *parent) :
    QAbstractTableModel(parent) { }

FilesModel::~FilesModel() { }

int FilesModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return static_cast<int>(count());
}

int FilesModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 3;
}

QVariant FilesModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return {};
    }

    const auto& fInfo = m_data.data.at(static_cast<size_t>(index.row()));

    if(!fInfo || !fInfo->fileInfo()) {
        return {};
    }

    switch(role){
    case NameRole: return fInfo->fileInfo()->fileName.c_str();
    case DateRole: return fInfo->fileInfo()->modificationDate.c_str();
    case SizeRole: return fInfo->fileInfo()->isDirectory ? "" : QString::number(fInfo->fileInfo()->size / 1024.0, 'f', 1);
    case DirectoryRole: return fInfo->fileInfo()->isDirectory;
    }

    return {};
}

QHash<int, QByteArray> FilesModel::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractTableModel::roleNames();
    roles[NameRole] = "name";
    roles[DateRole] = "date";
    roles[SizeRole] = "size";
    roles[DirectoryRole] = "isDirectory";

    return roles;
}

Qt::ItemFlags FilesModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    Qt::ItemFlags flgs = QAbstractTableModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
    return flgs;
}

size_t FilesModel::count() const {
    return m_data.data.size();
}

void FilesModel::InternalData::add(FileInfoWrapper* fInfo) {
    auto& ref = fInfo->fileInfo()->fileName;
    if(indices.find(ref) == indices.end()) {
        data.push_back(fInfo);
        indices[std::reference_wrapper<std::string>(fInfo->fileInfo()->fileName)] = data.size() - 1;
    }
}

void FilesModel::InternalData::remove(FileInfoWrapper* fInfo) {
    auto& ref = fInfo->fileInfo()->fileName;
    const auto iter = indices.find(ref);
    if(iter != indices.end()) {
        data.erase(data.begin() + iter->second);
        indices.erase(iter);
        updateIndicies();
    }
}

std::optional<size_t> FilesModel::InternalData::modify(FileInfoWrapper* fInfo) {
    auto& ref = fInfo->fileInfo()->fileName;
    const auto found = indices.find(ref);
    if(found != indices.end()) {
        auto idx = found->second;
        auto result = data[idx];
        result->setFileInfo(fInfo->fileInfo());
        indices.erase(found);
        indices[std::reference_wrapper<std::string>(fInfo->fileInfo()->fileName)] = idx;

        return  std::optional<size_t>(idx);
    }

    return {};
}

void FilesModel::InternalData::moveFrom(FileInfoWrapper* fInfo) {
    auto& ref = fInfo->fileInfo()->fileName;
    const auto iter = indices.find(ref);
    if(iter != indices.end()) {
        dataToRename = data[iter->second];
    }
}

std::optional<size_t> FilesModel::InternalData::moveTo(FileInfoWrapper* fInfo) {
    if(dataToRename) {
        auto& filename = dataToRename->fileInfo()->fileName;
        auto found = indices.find(filename);
        std::optional<size_t> result{};
        if(found != indices.end()) {
            auto idx = found->second;
            indices.erase(found);
            dataToRename->setFileInfo(fInfo->fileInfo());
            dataToRename = nullptr;
            indices[std::ref((data[idx])->fileInfo()->fileName)] = idx;
            result = idx;
        }

        return result;
    }

    return {};
}

void FilesModel::InternalData::clear() {
    indices.clear();
    std::for_each(data.begin(), data.end(), [](FileInfoWrapper* data){
        delete data;
    });
    data.clear();
}

std::optional<size_t> FilesModel::InternalData::distance(FileInfoWrapper* fInfo) const {
    auto& ref = fInfo->fileInfo()->fileName;
    const auto found = indices.find(ref);
    if(found != indices.end()) {
        return found->second;
    }

    return {};
}

void FilesModel::InternalData::updateIndicies() {
    indices.clear();
    size_t idx{};
    for(const auto& fInfo : data) {
        indices[std::ref(fInfo->fileInfo()->fileName)] = idx;
        ++idx;
    }
}

void FilesModel::handleAddEvent(FileInfoWrapper* fInfo) {
    auto dataSize = count();
    beginInsertRows(QModelIndex(), static_cast<int>(dataSize), static_cast<int>(dataSize));
    m_data.add(fInfo);
    endInsertRows();
    emit countChanged(static_cast<int>(count()));
}

void FilesModel::handleRemoveEvent(FileInfoWrapper* fInfo) {
    //поиск удаляемого файла по имени
    auto index = m_data.distance(fInfo);
    if(index != std::nullopt && index < count()) {
        beginRemoveRows(QModelIndex(), static_cast<int>(index.value()), static_cast<int>(index.value()));
        m_data.remove(fInfo);
        endRemoveRows();
        emit countChanged(static_cast<int>(count()));
    }

    delete fInfo;
}

void FilesModel::handleOldRenameEvent(FileInfoWrapper* fInfo) {
    m_data.moveFrom(fInfo);
    delete fInfo;
}

void FilesModel::handleNewRenameEvent(FileInfoWrapper* fInfo) {
    auto idx = m_data.moveTo(fInfo);

    if(fInfo) {
        delete fInfo;
    }

    if(idx != std::nullopt && idx.value() < m_data.data.size()) {
        emit dataChanged(createIndex(idx.value(), 0), createIndex(idx.value(), 0));
    }
}

void FilesModel::handleModifyEvent(FileInfoWrapper* fInfo) {
    auto idx = m_data.modify(fInfo);

    if(fInfo) {
        delete fInfo;
    }

    if(idx != std::nullopt && idx.value() < m_data.data.size()) {
        emit dataChanged(createIndex(idx.value(), 0), createIndex(idx.value(), 0));
    }
}

void FilesModel::handle(FileInfoWrapper* fInfo) {
    try{
        //something goes wrong
        if(!fInfo || !fInfo->fileInfo()) {
            return;
        }

        const auto& type = fInfo->fileInfo()->type;
        switch (type) {
            case api::DirectoryChangesType::Added:
                if(!fInfo->fileInfo()->isDirectory) {
                    handleAddEvent(fInfo);
                } break;
            case api::DirectoryChangesType::Removed: handleRemoveEvent(fInfo); break;
            case api::DirectoryChangesType::Modified: handleModifyEvent(fInfo); break;
            case api::DirectoryChangesType::OldRenamed: handleOldRenameEvent(fInfo); break;
            case api::DirectoryChangesType::NewRenamed: handleNewRenameEvent(fInfo); break;
            default: break;
        }

    } catch(const std::exception&) {
    }
}

void FilesModel::clear() {
    if(m_data.data.empty()){
        return;
    }
    beginRemoveRows(QModelIndex(), 0, static_cast<int>(count() - 1));
    m_data.clear();
    endRemoveRows();
}
