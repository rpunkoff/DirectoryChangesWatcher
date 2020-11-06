#include "directorychangeswatcher_tst.h"

#include "testmanager.h"

#include <fileinfo.h>
#include <directorychangeswatcher.h>

#include <QDir>
#include <QTest>
#include <QDebug>

#include <variant>
#include <thread>

using std::variant;
using std::shared_ptr;
using std::function;

using api::FileInfo;
using api::DirectoryChangesType;
using api::DirectoryChangesWatcher;

using utils::Event;

namespace testing {
DirectoryChangesWatcherTest::DirectoryChangesWatcherTest(QObject* parent)
    : BaseTest(QDir::homePath() + "/tests/", parent) {
    m_data.push_back({testDir() + "123/", directory_item_t::directory});
    m_data.push_back({testDir() + "test", directory_item_t::directory});
    m_data.push_back({testDir() + "test/dir", directory_item_t::directory});
    m_data.push_back({testDir() + "test/dir/ddd.dd", directory_item_t::file});
    m_data.push_back({testDir() + "tests.data.1", directory_item_t::file});
    m_data.push_back({testDir() + "tests.data.2", directory_item_t::file});
    m_data.push_back({testDir() + "tests.data.3", directory_item_t::file});
    m_data.push_back({testDir() + "tests.data.4", directory_item_t::file});
    m_data.push_back({testDir() + "123/tests.123.data.1", directory_item_t::file});
    m_data.push_back({testDir() + "123/tests.123.data.2", directory_item_t::file});
    m_data.push_back({testDir() + "123/tests.123.data.3", directory_item_t::file});
    m_data.push_back({testDir() + "123/tests.123.data.4", directory_item_t::file});
    m_data.push_back({testDir() + "123/tests.123.data.5", directory_item_t::file});
    m_data.push_back({testDir() + "123/tests.123.data.6", directory_item_t::file});
    m_data.push_back({testDir() + "123/ddd", directory_item_t::directory});
    m_data.push_back({testDir() + "test/прочее", directory_item_t::directory});
}

void DirectoryChangesWatcherTest::initTestCase() {
    removeTestDir();
    for(const auto& d : m_data) {
        if(d.type == directory_item_t::directory) {
            addDir(d.path);
        } else {
            addFile(d.path);
        }
    }
}

void DirectoryChangesWatcherTest::cleanupTestCase() {
    removeTestDir();
}

void TestCommon(const DirectoryChangesWatcher& watcher, bool signalHandled, int sizeActual, int sizeExpected){
    QCOMPARE(watcher.isStopped(), true);
    QCOMPARE(signalHandled, true);
    QVERIFY(sizeActual >= sizeExpected);
}

void TestItem(const shared_ptr<FileInfo>& fInfo, DirectoryChangesType type, bool isDirectory, QString fName) {
    QCOMPARE(fInfo->type, type);
    QCOMPARE(fInfo->isDirectory, isDirectory);
    if(fInfo->isDirectory) {
        if(fName.back() != '/') {
            fName.append('/');
        }
        if(fInfo->fileName.back() != '/') {
            fInfo->fileName += '/';
        }
    }

    QCOMPARE(fInfo->fileName, fName.toStdString());
}

using one_param_ev_t = Event<const QString&>;
using two_params_ev_t = Event<const QString&, const QString&>;
using fn_variant_t = variant<one_param_ev_t, two_params_ev_t>;

struct WatcherTestData {
    fn_variant_t fn{};
    size_t sleepAfter{};
    QString filename{};
    QString filename2{};
};

struct WatcherTestItemResults {
    bool isDir;
    DirectoryChangesType type;
    size_t index;
    QString filename;
};

class WatcherTest {
public:
    WatcherTest(const string& directory);

    void addTestData(WatcherTestData&& testFunction);
    void setResultsSize(size_t size);
    void addTestItemResult(WatcherTestItemResults&& testItemResults);
    void act();
    void checkResults();
private:
    DirectoryChangesWatcher m_watcher;
    bool m_signalHandled {false};
    vector<shared_ptr<FileInfo>> m_testFileInfos;
    vector<WatcherTestData> m_testData;
    size_t m_resultsSize{};
    vector<WatcherTestItemResults> m_testResults;
};

WatcherTest::WatcherTest(const string& directory):
    m_watcher(directory){
    m_watcher.sent.attachFunction([&testFileInfos = this->m_testFileInfos,
                                  &signalHandled = m_signalHandled](const std::shared_ptr<api::FileInfo>& fInfo) mutable {
                                      //this crutch was done because a windows os can send 2 or more notifications
                                      //for the same file with the same notification type
                                      auto found = std::find_if(std::begin(testFileInfos), std::end(testFileInfos),
                                                                [&fInfo](const std::shared_ptr<api::FileInfo>& info){
                                                                        return info->fileName == fInfo->fileName && info->type == fInfo->type;
                                                                });
                                      if(found == std::end(testFileInfos)) {
                                          testFileInfos.push_back(fInfo);
                                              if(!signalHandled) {
                                                  signalHandled = true;
                                              }
                                      }
                                      });

}

void WatcherTest::addTestData(WatcherTestData&& testData) {
    m_testData.push_back(std::move(testData));
}

void WatcherTest::setResultsSize(size_t size) {
    m_resultsSize = size;
}

void WatcherTest::addTestItemResult(WatcherTestItemResults&& testItemResults) {
    m_testResults.push_back(std::move(testItemResults));
}

void WatcherTest::act() {
    m_watcher.start();
    std::thread thread([&watcher = this->m_watcher]() { watcher.run(); });
    QTest::qSleep(50);
    for(auto& item : m_testData) {
        auto ptr = std::get_if<one_param_ev_t>(&item.fn);
        if(ptr) {
            (*ptr).raise(item.filename);
        } else {
            auto ptr = std::get_if<two_params_ev_t>(&item.fn);
            if(ptr) {
                 (*ptr).raise(item.filename, item.filename2);
            }
        }
        QTest::qSleep(static_cast<int>(item.sleepAfter));
    }
    m_watcher.stop();
    thread.join();
}

void WatcherTest::checkResults() {
    TestCommon(m_watcher, m_signalHandled, static_cast<int>(m_testFileInfos.size()), static_cast<int>(m_resultsSize));
    auto resultsSize = m_testResults.size();
    for(const auto& item : m_testResults) {
        QVERIFY(item.index < resultsSize);
        const auto& data = m_testFileInfos.at(item.index);
        TestItem(data, item.type, item.isDir, item.filename);
    }
}

void DirectoryChangesWatcherTest::testWatcherCanRunAndStop() {
    DirectoryChangesWatcher watcher(testDir().toStdString());

    QCOMPARE(watcher.isStopped(), true);
    watcher.start();
    QCOMPARE(watcher.isStopped(), false);
    watcher.stop();
    QCOMPARE(watcher.isStopped(), true);
}

template<typename T>
using OneParamMethodPtr = void (T::*)(const QString&);

template<typename T>
using TwoParamsMethodPtr = void (T::*)(const QString&, const QString&);

template<typename T, template <typename> typename PtrT, typename EventT>
EventT CreateEvent(T* inst, PtrT<T> ptr){
    EventT ev;
    ev.attachMember(inst, ptr);

    return ev;
}

void DirectoryChangesWatcherTest::testWatcherAddFile() {
    WatcherTest test(testDir().toStdString());

    QString fName = testDir() + "file123.f";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addFile), 50, fName});
    test.setResultsSize(2);
    test.addTestItemResult({false, DirectoryChangesType::Added, 0, fName});
    test.addTestItemResult({false, DirectoryChangesType::Modified, 1, fName});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAddDir() {
    WatcherTest test(testDir().toStdString());

    QString fName = testDir() + "directory";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addDir), 50, fName});
    test.setResultsSize(1);
    test.addTestItemResult({true, DirectoryChangesType::Added, 0, fName});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAddCyrillicFile() {
    WatcherTest test(testDir().toStdString());

    QString fName = testDir() + "файл123.f";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addFile), 50, fName});
    test.setResultsSize(2);
    test.addTestItemResult({false, DirectoryChangesType::Added, 0, fName});
    test.addTestItemResult({false, DirectoryChangesType::Modified, 1, fName});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAddCyrillicDir() {
    WatcherTest test(testDir().toStdString());

    QString fName = testDir() + "директория";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addDir), 50, fName});
    test.setResultsSize(1);
    test.addTestItemResult({true, DirectoryChangesType::Added, 0, fName});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAddFileInCyrillicPath() {
    WatcherTest test(testDir().toStdString());

    QString fName = m_data.back().path + "/file123.f";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addFile), 50, fName});
    test.setResultsSize(2);
    test.addTestItemResult({false, DirectoryChangesType::Added, 0, fName});
    test.addTestItemResult({false, DirectoryChangesType::Modified, 1, fName});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAddDirInCyrillicPath(){
    WatcherTest test(testDir().toStdString());

    QString fName = m_data.back().path + "/directory";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addDir), 50, fName});
    test.setResultsSize(1);
    test.addTestItemResult({true, DirectoryChangesType::Added, 0, fName});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAddDirAndFileToSamePath() {
    WatcherTest test(testDir().toStdString());

    QString fName1 = m_data.back().path + "/file123.f1";
    QString fName2 = m_data.back().path + "/directory1";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addFile), 50, fName1});
    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addDir), 50, fName2});
    test.setResultsSize(3);
    test.addTestItemResult({false, DirectoryChangesType::Added, 0, fName1});
    test.addTestItemResult({false, DirectoryChangesType::Modified, 1, fName1});
    test.addTestItemResult({true, DirectoryChangesType::Added, 2, fName2});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAddDirAndFileIntoAddedDir() {
    WatcherTest test(testDir().toStdString());

    QString fName1 = m_data.back().path + "/directory2/";
    QString fName2 = m_data.back().path + "/directory2/file234.f2";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addDir), 50, fName1});
    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addFile), 50, fName2});
    test.setResultsSize(3);
    test.addTestItemResult({true, DirectoryChangesType::Added, 0, fName1});
    test.addTestItemResult({false, DirectoryChangesType::Added, 1, fName2});
    test.addTestItemResult({false, DirectoryChangesType::Modified, 2, fName2});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAddAndRemoveFile() {
    WatcherTest test(testDir().toStdString());

    QString fName = m_data.back().path + "/filetoremove.f";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addFile), 50, fName});
    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::removeFile), 50, fName});
    test.setResultsSize(3);
    test.addTestItemResult({false, DirectoryChangesType::Added, 0, fName});
    test.addTestItemResult({false, DirectoryChangesType::Modified, 1, fName});
    test.addTestItemResult({false, DirectoryChangesType::Removed, 2, fName});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAddAndRemoveEmptyDir() {
    WatcherTest test(testDir().toStdString());

    QString fName = m_data.back().path + "/dirtoremove";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::addDir), 50, fName});
    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::removeDir), 50, fName});
    test.setResultsSize(2);
    test.addTestItemResult({true, DirectoryChangesType::Added, 0, fName});
    test.addTestItemResult({true, DirectoryChangesType::Removed, 1, fName});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherRemoveNotEmptyDir() {
    WatcherTest test(testDir().toStdString());

    QString fName = testDir() + + "123";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      OneParamMethodPtr,
                      one_param_ev_t>(this, &DirectoryChangesWatcherTest::removeDir), 800, fName});
    test.setResultsSize(1);

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherModifyFile() {
    WatcherTest test(testDir().toStdString());

    QString fName = testDir() + "tests.data.1";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      TwoParamsMethodPtr,
                      two_params_ev_t>(this, &DirectoryChangesWatcherTest::appendDataToFile), 50, fName, "test data"});
    test.setResultsSize(1);
    test.addTestItemResult({false, DirectoryChangesType::Modified, 0, fName});

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherRenameFile() {
    WatcherTest test(testDir().toStdString());

    QString fNameOld = testDir() + "tests.data.1";
    QString fNameNew = testDir() + "test/tests.data.1";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      TwoParamsMethodPtr,
                      two_params_ev_t>(this, &DirectoryChangesWatcherTest::renamePath), 50, fNameOld, fNameNew});
    test.setResultsSize(2);
#ifdef Q_OS_WIN
    test.addTestItemResult({false, DirectoryChangesType::Removed, 0, fNameOld});
    test.addTestItemResult({false, DirectoryChangesType::Added, 1, fNameNew});
#else
    test.addTestItemResult({false, DirectoryChangesType::OldRenamed, 0, fNameOld});
    test.addTestItemResult({false, DirectoryChangesType::NewRenamed, 1, fNameNew});
#endif

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherRenameEmptyDir() {
    addDir(testDir() + "emptydir");

    WatcherTest test(testDir().toStdString());

    QString fNameOld = testDir() + "emptydir";
    QString fNameNew = testDir() + "test/emptydir";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      TwoParamsMethodPtr,
                      two_params_ev_t>(this, &DirectoryChangesWatcherTest::renamePath), 50, fNameOld, fNameNew});
    test.setResultsSize(2);
#ifdef Q_OS_WIN
    test.addTestItemResult({true, DirectoryChangesType::Removed, 0, fNameOld});
    test.addTestItemResult({true, DirectoryChangesType::Added, 1, fNameNew});
#else
    test.addTestItemResult({true, DirectoryChangesType::OldRenamed, 0, fNameOld});
    test.addTestItemResult({true, DirectoryChangesType::NewRenamed, 1, fNameNew});
#endif

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherRenameNotEmptyDir() {
    addDir(testDir() + "newdir");
    addFile(testDir() + "newdir/file1");
    addFile(testDir() + "newdir/file2");
    addFile(testDir() + "newdir/file3");
    addDir(testDir() + "newdir/dir");
    addFile(testDir() + "newdir/dir/file4");

    WatcherTest test(testDir().toStdString());

    QString fNameOld = testDir() + "newdir";
    QString fNameNew = testDir() + "test/newdir";

    test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                      TwoParamsMethodPtr,
                      two_params_ev_t>(this, &DirectoryChangesWatcherTest::renamePath), 100, fNameOld, fNameNew});
    test.setResultsSize(2);
#ifdef Q_OS_WIN
    test.addTestItemResult({true, DirectoryChangesType::Removed, 0, fNameOld});
    test.addTestItemResult({true, DirectoryChangesType::Added, 1, fNameNew});
#else
    test.addTestItemResult({true, DirectoryChangesType::OldRenamed, 0, fNameOld});
    test.addTestItemResult({true, DirectoryChangesType::NewRenamed, 1, fNameNew});
#endif

    test.act();

    test.checkResults();
}

void DirectoryChangesWatcherTest::testWatcherAdd100Files() {
    WatcherTest test(testDir().toStdString());

    QString path = testDir() + "test/dir/";

    for(size_t i = 0; i < 100; ++i) {
        test.addTestData({CreateEvent<DirectoryChangesWatcherTest,
                          OneParamMethodPtr,
                          one_param_ev_t>(this, &DirectoryChangesWatcherTest::addFile), 50, path + QString("test_file_%1").arg(i)});
    }

    test.setResultsSize(200);

    for(size_t i = 0, j = 0; i < 199; i+=2, ++j) {
        test.addTestItemResult({false, DirectoryChangesType::Added, i, path + QString("test_file_%1").arg(j)});
        test.addTestItemResult({false, DirectoryChangesType::Modified, i + 1, path + QString("test_file_%1").arg(j)});
    }

    test.act();

    test.checkResults();
}

static const DirectoryChangesWatcherTest directoryChangesWatcherTest;
}
