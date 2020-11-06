#include "../include/qdirectorychangeswatcher.h"
#include "../include/fileinfowrapper.h"

#include <directorychangeswatcher.h>
#include <directorychangesexception.h>

using api::DirectoryChangesWatcher;
using api::DirectoryChangesException;

using std::make_shared;

class QDirectoryChangesWatcherPrivate {
    Q_DECLARE_PUBLIC(QDirectoryChangesWatcher)

public:
     QDirectoryChangesWatcherPrivate(const QString &directory, QDirectoryChangesWatcher* q);
    ~QDirectoryChangesWatcherPrivate();

    void onChanged(std::shared_ptr<api::FileInfo> fInfo);

private:
    QDirectoryChangesWatcher *q_ptr{nullptr};
    QString m_directory;
    std::unique_ptr<DirectoryChangesWatcher> m_watcher {};
};

QDirectoryChangesWatcherPrivate::QDirectoryChangesWatcherPrivate(const QString &directory, QDirectoryChangesWatcher* q) :
    q_ptr(q),
    m_directory(directory){ }

QDirectoryChangesWatcherPrivate::~QDirectoryChangesWatcherPrivate() {
    Q_Q(QDirectoryChangesWatcher);
    q->stop();
}

void QDirectoryChangesWatcherPrivate::onChanged(std::shared_ptr<api::FileInfo> fInfo) {
    Q_Q(QDirectoryChangesWatcher);
    emit q->notify(new FileInfoWrapper(std::move(fInfo)));
}

QDirectoryChangesWatcher::QDirectoryChangesWatcher(const QString &directory, QObject* parent) :
    QObject(parent),
    d_ptr(new QDirectoryChangesWatcherPrivate(directory, this)){
}

QDirectoryChangesWatcher::~QDirectoryChangesWatcher(){
    delete d_ptr;
}

void QDirectoryChangesWatcher::run() {
    Q_D(QDirectoryChangesWatcher);

    d->m_watcher = std::make_unique<DirectoryChangesWatcher>(d->m_directory.toStdString());
    d->m_watcher->sent.attachMember(d, &QDirectoryChangesWatcherPrivate::onChanged);

    if(!d->m_watcher->start()) {
        emit failed("An internal error has occured");
        return;
    }

    try {
        d->m_watcher->run();
    } catch(const DirectoryChangesException& e) {
        emit failed(e.what());
        return;
    }

    emit finished();
}

void QDirectoryChangesWatcher::stop() {
    Q_D(QDirectoryChangesWatcher);

    if(!d->m_watcher) {
        return;
    }

    if(!d->m_watcher->isStopped()) {
        d->m_watcher->stop();
        d->m_watcher.reset();
    }
}
