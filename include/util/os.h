//
// Created by Saman on 11.10.23.
//

#ifndef DOUGHNUT_OS_H
#define DOUGHNUT_OS_H

// Full list:
// https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define OS_WINDOWS
#elif __APPLE__
#define OS_MAC
#elif __linux__
#define OS_LINUX
#else
#define OS_UNKNOWN
#endif

#endif //DOUGHNUT_OS_H
