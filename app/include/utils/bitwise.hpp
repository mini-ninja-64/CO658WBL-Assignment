#pragma once

#define UINT8_TO_UINT32(pointer, offset) ((uint32_t) pointer[0+offset] << 24 | \
                                          (uint32_t) pointer[1+offset] << 16 | \
                                          (uint32_t) pointer[2+offset] << 8  | \
                                          (uint32_t) pointer[3+offset] << 0)

#define UINT32_TO_UINT8(number) (uint8_t) (number >> 24) , \
                                (uint8_t) (number >> 16) , \
                                (uint8_t) (number >> 8)  , \
                                (uint8_t) (number >> 0)
