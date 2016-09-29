/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#ifndef VS_UTILITIES_H
#define VS_UTILITIES_H

#include "stdint.h"

// 1byte to uint32
int32_t read1Bytetoint32(uint8_t *buffer, int32_t buffLen, int32_t *pos, uint32_t *outVal);

// 1 byte to uint8_t
int32_t read1Byteto1Byte(uint8_t *buffer, int32_t buffLen, int32_t *pos, uint8_t *outVal);

// 4byte to uint32
int32_t read4Bytetoint32(uint8_t *buffer, int32_t buffLen, int32_t *pos, uint32_t *outVal);

int32_t readXBytestoint32(uint8_t *buffer, int32_t buffLen, int32_t xBytes, int32_t *pos, uint32_t *outVal);

int32_t readShortToShort(uint8_t *buffer, int32_t buffLen, int32_t *pos, uint16_t *outVal);

#endif //VS_UTILITIES_H
