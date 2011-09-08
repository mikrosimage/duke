/*
 * PageSize.h
 *
 *  Created on: Jan 9, 2011
 *      Author: Guillaume Chatelet
 */

#include "PageSize.h"

#include <cstdio>

#ifdef _WIN32
#include <windows.h>
unsigned long getPageSize() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
}
#else
#include <unistd.h>
unsigned long getPageSize() {
    return sysconf(_SC_PAGESIZE);
}
#endif
