//
// Created by Sam on 2023-04-11.
//

#ifndef REALTIME_CELL_COLLAPSE_PRINTER_H
#define REALTIME_CELL_COLLAPSE_PRINTER_H

#include "preprocessor.h"

#include <iostream>
#include <stdexcept>

#define INFO_PRINTING
#define DEBUG_PRINTING
//#define VERBOSE_PRINTING

#if(defined(NDEBUG) || !defined(INFO_PRINTING))
#define info(stream)
#else
#define info(stream) if(false) std::cout << stream << "\n"
#endif

#if(defined(NDEBUG) || !defined(DEBUG_PRINTING))
#define debug(stream)
#else
#define debug(stream) if(false) std::cout << stream << "\n"
#endif

#if(defined(NDEBUG) || !defined(VERBOSE_PRINTING))
#define verbose(stream)
#else
#define verbose(stream) if(false) std::cout << stream << "\n"
#endif

#define trace(name, block) { \
    auto _trace_before = Timer::now(); \
    block; \
    auto _trace_after = Timer::now(); \
    auto _duration = std::to_string(Timer::duration(_trace_before, _trace_after)); \
    info(name << ": " << _duration ENDL; \
}

#endif //REALTIME_CELL_COLLAPSE_PRINTER_H
