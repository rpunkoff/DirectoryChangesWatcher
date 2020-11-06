#ifndef BASE_TEST_H
#define BASE_TEST_H

#include <QObject>
#include <QString>

namespace testing {
class BaseTest : public QObject {
    Q_OBJECT
public:
    BaseTest(const QString& path, QObject* parent = nullptr);
    ~BaseTest() override = default;

    virtual void initTestCase() = 0;
    virtual void cleanupTestCase() = 0;

protected:
    void removeTestDir();

    const QString& testDir() const;

    enum class directory_item_t {
        directory,
        file
    };

    void addDir(const QString& path);
    void addFile(const QString& path);

    void removeFile(const QString& path);
    void removeDir(const QString& path);

    void appendDataToFile(const QString& path, const QString& data);
    void renamePath(const QString& oldPath, const QString& newPath);

private:
    QString m_testDir;
};
}

#endif // BASE_TEST_H
