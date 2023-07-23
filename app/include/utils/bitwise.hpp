#pragma once

#define UINT8_TO_UINT32(pointer, offset) ((uint32_t) pointer[0+offset] << 24 | \
                                          (uint32_t) pointer[1+offset] << 16 | \
                                          (uint32_t) pointer[2+offset] << 8  | \
                                          (uint32_t) pointer[3+offset] << 0)

#define UINT32_TO_UINT8(number) (uint8_t) (number >> 24) , \
                                (uint8_t) (number >> 16) , \
                                (uint8_t) (number >> 8)  , \
                                (uint8_t) (number >> 0)

#define UINT32_TO_UINT8_ARRAY(array, offset, number) array[offset]     = (uint8_t) (number >> 24); \
                                                     array[offset + 1] = (uint8_t) (number >> 16); \
                                                     array[offset + 2] = (uint8_t) (number >> 8);  \
                                                     array[offset + 3] = (uint8_t) (number >> 0)
