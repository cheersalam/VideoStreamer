#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include "memoryManagement.h"

int32_t getPageSize() {
    return (int32_t)getpagesize();
}


int32_t allignedAlloc(void **buffer, size_t bytes, size_t *bytesAllocated) {
    int32_t err = -1;
    int32_t pageSize = getPageSize();
    size_t alignedLen = ALIGN_LEN(bytes, pageSize);

    printf("allignedAlloc request for bytes = %zu allocated = %zu \n", bytes, *bytesAllocated);
    if(0 == posix_memalign(buffer, pageSize, alignedLen)) {
    	memset(*buffer, 0, alignedLen);
        *bytesAllocated = alignedLen;
        err = 0;
        printf("allignedAlloc success for bytes = %zu allocated = %zu \n", bytes, *bytesAllocated);
    }
    else {
        printf("posix_memalign alligned alloc failed for bytes = %zu \n", bytes);
    }
    return err;
}

int32_t allignedRealloc(void **buffer, size_t currentSize, size_t newbytes, size_t *bytesAllocated) {
    int32_t err = -1;
    void *lBuffer = NULL;
    int32_t pageSize = getPageSize();
    size_t alignedLen = ALIGN_LEN(newbytes, pageSize);

    printf("allignedRealloc request for currentSize = %zu newbytes = %zu bytesAllocated = %zu\n", currentSize, newbytes, *bytesAllocated);
    if(0 == posix_memalign(&lBuffer, pageSize, alignedLen)) {
    	memset(lBuffer, 0, alignedLen);
        memcpy(lBuffer, *buffer, currentSize);
        free(*buffer);
        *buffer = lBuffer;
        *bytesAllocated = alignedLen;
        err = 0;
        printf("aligned realloc success for newbytes = %zu allocated = %zu \n", newbytes, *bytesAllocated);
    }
    else {
        printf("posix_memalign in allignedReallocfailed for newbytes = %zu \n", newbytes);
    }
    return err;
}

