
#include "application.h"
#include "core/scheduler.h"

#include <iostream>

int main() {
//#ifndef NDEBUG
//    {
//        trace_scope("Unit Tests");
//        ECS2::testEntityManager();
//        ECS2::testSystemManager();
//    }
//#endif

    Doughnut::Scheduler scheduler{};
    {
        trace_scope("Schedule 2 tasks");
        scheduler.queue({
                                []() {
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    std::cout << "Hi!\n";
                                },
                                []() {
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    std::cout << "Hello!\n";
                                }
        });
    }
    return 0;

    Doughnut::Application app{"Hello World!"};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}