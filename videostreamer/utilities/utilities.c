/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#include "stdio.h"
#include "stdint.h"
#include "utilities.h"
#include <sys/time.h>

// 1byte to uint32
int32_t read1Bytetoint32(uint8_t *buffer, int32_t buffLen, int32_t *pos, uint32_t *outVal) {
	if ( (NULL == buffer) || (*pos > buffLen)) {
		return -1;
	}
	*outVal = (uint32_t)buffer[(*pos)++];
	return 0;
}

// 1 byte to uint8_t
int32_t read1Byteto1Byte(uint8_t *buffer, int32_t buffLen, int32_t *pos, uint8_t *outVal) {
	if ((NULL == buffer) || (*pos > buffLen)) {
		return -1;
	}
	*outVal = buffer[(*pos)++];
	return 0;
}

// 4byte to uint32
int32_t read4Bytetoint32(uint8_t *buffer, int32_t buffLen, int32_t *pos, uint32_t *outVal) {
	if ((NULL == buffer) || ((*pos + 4) > buffLen)) {
		return -1;
	}
	*outVal = ((buffer[*pos] << 24) | (buffer[*pos + 1] << 16) | (buffer[*pos + 2] << 8) | buffer[*pos + 3]);
	*pos += 4;
	return 0;
}

int32_t readShortToShort(uint8_t *buffer, int32_t buffLen, int32_t *pos, uint16_t *outVal) {
	if ((NULL == buffer) || ((*pos + 2) > buffLen)) {
		return -1;
	}
	*outVal = (buffer[*pos] << 8) | buffer[*pos + 1];
	*pos += 2;
	return 0;
}

int32_t readXBytestoint32(uint8_t *buffer, int32_t buffLen, int32_t xBytes, int32_t *pos, uint32_t *outVal) {
	int32_t i = 0;
	*outVal = 0;
	if ((NULL == buffer) || (xBytes > 4) || (xBytes <= 0) || ((*pos + xBytes) > buffLen)) {
		return -1;
	}
	for (i = 0; i < xBytes; i++) {
		*outVal = (*outVal) | (buffer[(*pos)++] << (8 * i));
	}
	return 0;
}


uint64_t getMilliSecondsSinceEpoch() {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (uint64_t)(tv.tv_sec * 1000) + (uint64_t)(tv.tv_usec/1000);
}





