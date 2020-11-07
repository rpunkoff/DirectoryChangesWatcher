#ifndef QT_DIRECTORY_CHANGES_WATCHER_H
#define QT_DIRECTORY_CHANGES_WATCHER_H

#include <QObject>
#include <QString>

class FileInfoWrapper;
class QDirectoryChangesWatcherPrivate;

class QDirectoryChangesWatcher : public QObject {
	Q_OBJECT
public:
    explicit QDirectoryChangesWatcher(const QString& directory, QObject* parent = nullptr);
    ~QDirectoryChangesWatcher() override;

public slots:
    void run();
    void stop();

signals:
    void notify(FileInfoWrapper* fInfo);
    void failed(const QString& errorString);
    void finished();

private:
    Q_DECLARE_PRIVATE(QDirectoryChangesWatcher)
    QDirectoryChangesWatcherPrivate* d_ptr {nullptr};
};

#endif //QT_DIRECTORY_CHANGES_WATCHER_H
