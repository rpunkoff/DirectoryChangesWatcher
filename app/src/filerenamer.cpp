#include "../include/filerenamer.h"

#include <filerenamer.h>

FileRenamer::FileRenamer(QObject* parent) :
    QObject(parent) { }

void FileRenamer::rename(const QString &oldName, const QString &newName) {
    api::Rename(oldName.toStdWString(), newName.toStdWString());
}
