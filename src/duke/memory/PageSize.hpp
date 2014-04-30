#pragma once

/**
 * Usually page size is 4096 so you can safely use
 * the following constants
 * http://en.wikipedia.org/wiki/Page_(computer_memory)
 */

#define PAGE_SIZE_BITS 12                // 2^12 = 4096
#define PAGE_SIZE (1 << PAGE_SIZE_BITS)  // 4096

/**
 * In some rare occasions the page size might be different
 * you can check for page size by calling the runtime function
 * getPageSize below.
 */
unsigned long getPageSize();
