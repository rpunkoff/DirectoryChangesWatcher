#include <QCoreApplication>
#include <QTest>

#include "src/testmanager.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    return testing::TestManager::instance().runTests(argc, argv);
}
