#ifndef QT_DIRECTORY_CONTENT_READER_H
#define QT_DIRECTORY_CONTENT_READER_H

#include <QObject>
#include <QString>

class FileInfoWrapper;
class QDirectoryContentReaderPrivate;

class QDirectoryContentReader : public QObject {
    Q_OBJECT
public:
    QDirectoryContentReader(const QString& directory, QObject* parent = nullptr);
    ~QDirectoryContentReader() override;

public slots:
    void run();

signals:
    void notify(FileInfoWrapper*);
    void failed(const QString& errorString);
	void finished();

private:
    Q_DECLARE_PRIVATE(QDirectoryContentReader)
    QDirectoryContentReaderPrivate* d_ptr;
};

#endif //QT_DIRECTORY_CONTENT_READER_H
