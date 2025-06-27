
#include "io/logger.h"
#include "test_util.h"

TEST_REGISTRY

int main() {
    dn::log::init(true, false, false, false);

    for (auto &test: TQ) {
        test();
    }

    dn::log::i(errorCounter, "/", testCaseCounter, "test cases failed in", testCounter, "tests");

    return 0;
}