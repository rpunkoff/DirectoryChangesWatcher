#include "directorycontentreader_tst.h"

#include "testmanager.h"

#include <directorycontentreader.h>
#include <directorychangesexception.h>

#include <QDir>
#include <QTest>

using api::DirectoryContentReader;

namespace testing {
DirectoryContentReaderTest::DirectoryContentReaderTest(QObject* parent)
    : BaseTest(QDir::homePath() + "/tests/", parent) {
    m_data.push_back({12, testDir(), "", directory_item_t::directory});
    m_data.push_back({7, testDir(), "123/", directory_item_t::directory});
    m_data.push_back({1, testDir(), "test", directory_item_t::directory});
    m_data.push_back({1, testDir(), "test/dir", directory_item_t::directory});
    m_data.push_back({1, testDir(), "test/dir/ddd.dd", directory_item_t::file});
    m_data.push_back({1, testDir(), "tests.data.1", directory_item_t::file});
    m_data.push_back({1, testDir(), "tests.data.2", directory_item_t::file});
    m_data.push_back({1, testDir(), "tests.data.3", directory_item_t::file});
    m_data.push_back({1, testDir(), "tests.data.4", directory_item_t::file});
    m_data.push_back({1, testDir() + "123/", "tests.123.data.1", directory_item_t::file});
    m_data.push_back({1, testDir() + "123/", "tests.123.data.2", directory_item_t::file});
    m_data.push_back({1, testDir() + "123/", "tests.123.data.3", directory_item_t::file});
    m_data.push_back({1, testDir() + "123/", "tests.123.data.4", directory_item_t::file});
    m_data.push_back({1, testDir() + "123/", "tests.123.data.5", directory_item_t::file});
    m_data.push_back({1, testDir() + "123/", "tests.123.data.6", directory_item_t::file});
    m_data.push_back({0, testDir() + "123/", "ddd", directory_item_t::directory});
}

void DirectoryContentReaderTest::initTestCase() {
    removeTestDir();
    for(const auto& d : m_data) {
        if(d.type == directory_item_t::directory) {
            addDir(d.directory + d.relative);
        } else {
            addFile(d.directory + d.relative);
        }
    }
}

void DirectoryContentReaderTest::cleanupTestCase() {
    removeTestDir();
}

void DirectoryContentReaderTest::testReadUnexistedDirectory_data() {
    QTest::addColumn<QString>("directory");
    QTest::addColumn<QString>("relative");

    QTest::newRow("is_not_directory") << QString("testdir") << "";
    QTest::newRow("directory_does_not_exist") << testDir() + "testdir123" << "";
    QTest::newRow("directory_does_not_exist") << testDir() << "testdir123";
}

void DirectoryContentReaderTest::testReadUnexistedDirectory() {
    QFETCH(QString, directory);
    QFETCH(QString, relative);

    directory.prepend(testDir());

    QVERIFY_EXCEPTION_THROWN(DirectoryContentReader::read(directory.toStdString(), relative.toStdString()),
                             api::DirectoryChangesException);
}

void DirectoryContentReaderTest::testReadExistedDirectory_data() {
    QTest::addColumn<int>("count");
    QTest::addColumn<QString>("directory");
    QTest::addColumn<QString>("relative");

    for(const auto& d : m_data) {
        if(d.type == directory_item_t::directory) {
            QString str = d.directory + d.relative;
            QTest::newRow(str.toStdString().c_str()) << d.count << d.directory << d.relative;
        }
    }
}

void DirectoryContentReaderTest::testReadExistedDirectory() {
    QFETCH(int, count);
    QFETCH(QString, directory);
    QFETCH(QString, relative);

    auto files = DirectoryContentReader::read(directory.toStdString(), relative.toStdString());

    QCOMPARE(static_cast<size_t>(count), files.size());
}

static const DirectoryContentReaderTest directoryContentReaderTest;
}

