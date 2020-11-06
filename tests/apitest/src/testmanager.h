#ifndef TEST_MANAGER_H
#define TEST_MANAGER_H

#include <vector>

using std::vector;

namespace testing {
class BaseTest;

class TestManager final {
public:
    static int runTests(int argc, char* arvg[]);
    static void addTest(BaseTest* test);
    static TestManager &instance();
    ~TestManager();

private:
    TestManager() = default;
    vector<BaseTest*> m_tests;
};
}

#endif // TEST_MANAGER_H
