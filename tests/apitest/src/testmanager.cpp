#include "testmanager.h"
#include <QDebug>
#include <QTest>

#include "basetest.h"

namespace testing {
int TestManager::runTests(int argc, char *argv[]) {
    QStringList failedTests;

    for (const auto& test : instance().m_tests) {
        if (QTest::qExec(test, argc, argv)) {
            failedTests << test->metaObject()->className();
        }
    }

    if (!failedTests.isEmpty()) {
        qWarning() << "#######################################";
        qWarning() << "TEST FAILURES DETECTED:";

        for (auto f : failedTests) {
            qWarning() << " - " << f;
        }

        qWarning() << "#######################################";
    }

    return failedTests.count();
}

void TestManager::addTest(BaseTest *test) {
    if (test and qobject_cast<QObject *>(test)) {
        instance().m_tests.push_back(test);
    }
}

TestManager &TestManager::instance() {
    static TestManager inst;

    return inst;
}

TestManager::~TestManager() { }
}
