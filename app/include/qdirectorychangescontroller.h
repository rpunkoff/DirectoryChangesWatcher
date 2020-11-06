#ifndef QT_DIRECTORY_CHANGES_CONTROLLER_H
#define QT_DIRECTORY_CHANGES_CONTROLLER_H

#include <QObject>

class FileInfoWrapper;
class QDirectoryChangesControllerPrivate;

class QDirectoryChangesController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
public:
    QDirectoryChangesController(QObject* parent = nullptr);
    ~QDirectoryChangesController();

    Q_INVOKABLE void run(const QString& directory);
    Q_INVOKABLE void stop();

    bool running() const;

signals:
    void notify(FileInfoWrapper* fInfo);
    void failed(const QString& errorString);
    void finished();
    void runningChanged(bool running);

private:
    void setRunning(bool running);

    Q_DECLARE_PRIVATE(QDirectoryChangesController)
    QDirectoryChangesControllerPrivate* d_ptr;

private slots:
    void stopReader();
};

Q_DECLARE_METATYPE(QDirectoryChangesController *)

#endif //QT_DIRECTORY_CHANGES_CONTROLLER_H
