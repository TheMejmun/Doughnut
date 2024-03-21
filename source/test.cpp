
#include "io/logger.h"
#include "util/timer.h"
#include "ecs/system_manager.h"
#include "ecs/entity_manager.h"

int main() {
    dn::log::init(true, false, false, true);

#ifdef NDEBUG
    // In case assert was used instead of dn::require
    dn::log::i("Do not run tests in Release mode.");
    return 1;
#endif

    trace_scope("Unit Tests")

    dn::testEntityManager();
    dn::testSystemManager();
    dn::testScheduler();

    // throw std::runtime_error("Hello World");
    return 0;
}