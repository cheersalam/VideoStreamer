#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ALIGN_LEN(len, ALIGN_TO) ((len + (ALIGN_TO - 1)) & ~(ALIGN_TO - 1))


int32_t allignedAlloc(void **buffer, size_t bytes, size_t *bytesAllocated);
int32_t allignedRealloc(void **buffer, size_t currentSize, size_t newbytes, size_t *bytesAllocated);

