#include "../include/qdirectorycontentreader.h"
#include "../include/fileinfowrapper.h"

#include <directorycontentreader.h>
#include <directorychangesexception.h>

#include <atomic>

using api::DirectoryContentReader;
using api::DirectoryChangesException;

class QDirectoryContentReaderPrivate {
     Q_DECLARE_PUBLIC(QDirectoryContentReader)

public:
    QDirectoryContentReaderPrivate(const QString& directory, QDirectoryContentReader* q);
    ~QDirectoryContentReaderPrivate() = default;

private:
    QDirectoryContentReader* q_ptr;
    QString m_directory;
    std::atomic_bool m_isRead{false};
};

QDirectoryContentReaderPrivate::QDirectoryContentReaderPrivate(const QString& directory, QDirectoryContentReader* q) :
    q_ptr(q), m_directory(directory) { }

QDirectoryContentReader::QDirectoryContentReader(const QString &directory, QObject* parent):
    QObject(parent),
    d_ptr(new QDirectoryContentReaderPrivate(directory, this)) { }

QDirectoryContentReader::~QDirectoryContentReader(){
    delete d_ptr;
}

void QDirectoryContentReader::run() {
    Q_D(QDirectoryContentReader);
    d->m_isRead = true;
    try{
        auto data = DirectoryContentReader::read(d->m_directory.toStdString());
        d->m_isRead = false;

        for(auto item : data){
            emit notify(new FileInfoWrapper(std::move(item)));
        }
    } catch(const DirectoryChangesException& e) {
        d->m_isRead = false;
        emit failed(e.what());
    }

    emit finished();
}
