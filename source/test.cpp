
#include "io/logger.h"
#include "application.h"
#include "core/scheduler.h"
#include "io/resource_pool.h"
#include "graphics/renderer_v2.h"

#include <iostream>

int main() {
    dn::log::init(true, false, false, true);

    trace_scope("Unit Tests")

    dn::testEntityManager();
    dn::testSystemManager();
    dn::testScheduler();
}