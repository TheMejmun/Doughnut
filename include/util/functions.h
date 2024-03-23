//
// Created by Sam on 2024-03-19.
//

#ifndef DOUGHNUT_FUNCTIONS_H
#define DOUGHNUT_FUNCTIONS_H

#if __has_include("source_location")
#define SOURCE_LOCATION_ENABLED
#include <source_location>
#endif

#ifdef SOURCE_LOCATION_ENABLED
#define this_function std::source_location::current().function_name()
#else
#define this_function __PRETTY_FUNCTION__
#endif

#endif //DOUGHNUT_FUNCTIONS_H
