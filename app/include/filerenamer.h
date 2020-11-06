#ifndef FILE_RENAMER_QT_H
#define FILE_RENAMER_QT_H

#include <QObject>
#include <QString>

class FileRenamer : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(FileRenamer)
public:
    FileRenamer(QObject* parent = nullptr);
    ~FileRenamer() override = default;

    Q_INVOKABLE void rename(const QString& oldName, const QString& newName);
};



#endif //FILE_RENAMER_QT_H
