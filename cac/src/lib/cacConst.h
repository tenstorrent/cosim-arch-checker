#ifndef CAC_CONST_H
#define CAC_CONST_H

#include "cacTypeDef.h"

const sizenBitT UNIT_BIT_NUM = 64;
const sizenBitT VEC_128 = 128;
const sizenBitT VEC_256 = 256;
const sizenBitT VEC_512 = 512;
const size8BytesT RST_8_BYTE_VALUE[] = {0x0};

// 0:RT_FIX, 1:RT_FLT, 2:RT_X, 3: RT_PAS, 4: RT_VEC
const unsigned int REGISTER_RT_FIX_ENCODING = 0;
const unsigned int REGISTER_RT_FLT_ENCODING = 1;
const unsigned int REGISTER_RT_VEC_ENCODING = 4;

#endif
