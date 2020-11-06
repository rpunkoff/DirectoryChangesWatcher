#ifndef DIRECTORY_CHANGES_WATCHER_TEST_H
#define DIRECTORY_CHANGES_WATCHER_TEST_H

#include "basetest.h"

#include <QString>

namespace testing {
class DirectoryChangesWatcherTest : public BaseTest {
    Q_OBJECT
public:
    explicit DirectoryChangesWatcherTest(QObject* parent = nullptr);
    ~DirectoryChangesWatcherTest() override = default;

private slots:
    void initTestCase() override;
    void cleanupTestCase() override;

    void testWatcherCanRunAndStop();

    void testWatcherAddFile();
    void testWatcherAddDir();
    void testWatcherAddCyrillicFile();
    void testWatcherAddCyrillicDir();
    void testWatcherAddFileInCyrillicPath();
    void testWatcherAddDirInCyrillicPath();
    void testWatcherAddDirAndFileToSamePath();
    void testWatcherAddDirAndFileIntoAddedDir();

    void testWatcherAddAndRemoveFile();
    void testWatcherAddAndRemoveEmptyDir();
    void testWatcherRemoveNotEmptyDir();

    void testWatcherModifyFile();

    void testWatcherRenameFile();
    void testWatcherRenameEmptyDir();
    void testWatcherRenameNotEmptyDir();

    void testWatcherAdd100Files();

private:
    struct test_data_t {
        QString path;
        directory_item_t type;
    };

    std::vector<test_data_t> m_data;
};
}

#endif //DIRECTORY_CHANGES_WATCHER_TEST_H
