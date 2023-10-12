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
#define info(stream) std::cout << stream << "\n"
#endif

#if(defined(NDEBUG) || !defined(DEBUG_PRINTING))
#define debug(stream)
#else
#define debug(stream) std::cout << stream << "\n"
#endif

#if(defined(NDEBUG) || !defined(VERBOSE_PRINTING))
#define verbose(stream)
#else
#define verbose(stream) std::cout << stream << "\n"
#endif

#endif //REALTIME_CELL_COLLAPSE_PRINTER_H
