/*
 * MemoryUtils.cpp
 *
 *  Created on: 2 avr. 2012
 *      Author: Guillaume Chatelet
 */

#ifdef _WIN32
#include <windows.h>
unsigned long long getTotalSystemMemory() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}
#else
#include <unistd.h>
unsigned long long getTotalSystemMemory()
{
    const unsigned long long pages = sysconf(_SC_PHYS_PAGES);
    const unsigned long long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}
#endif
