#ifndef DIRECTORY_CONTENT_READER_TEST_H
#define DIRECTORY_CONTENT_READER_TEST_H

#include "basetest.h"

#include <vector>

namespace testing {
class DirectoryContentReaderTest : public BaseTest {
    Q_OBJECT
public:
    explicit DirectoryContentReaderTest(QObject* parent = nullptr);
    ~DirectoryContentReaderTest() override = default;

private slots:
    void initTestCase() override;
    void cleanupTestCase() override;

   void testReadUnexistedDirectory_data();
   void testReadUnexistedDirectory();

    void testReadExistedDirectory_data();
    void testReadExistedDirectory();

private:
    struct test_data_t {
        int count{};
        QString directory;
        QString relative;
        directory_item_t type;
    };

    std::vector<test_data_t> m_data;
};
}

#endif //DIRECTORY_CONTENT_READER_TEST_H
