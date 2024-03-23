
#include "io/logger.h"
#include "test_util.h"

TEST_REGISTRY

int main() {
    dn::log::init(true, false, false, false);

    for (auto &test: TQ) {
        test();
    }

    dn::log::i("Tests executed:", TQ.size());
    dn::log::i("Successful test cases:", testCounter - errorCounter, "/", testCounter);

    return 0;
}