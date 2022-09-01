#ifndef CAC_TYPEDEF_H
#define CAC_TYPEDEF_H

#include <vector>

typedef unsigned char size1ByteT;
typedef unsigned short int size2BytesT;
typedef unsigned int size4BytesT;
typedef unsigned long long int size8BytesT;

typedef size2BytesT threadT;
typedef size2BytesT stateIdT;
typedef size2BytesT sizenBitT;
typedef size8BytesT fuzzMaskT;
typedef size8BytesT unitDataT;

typedef std::vector<size8BytesT> dataVectorT;

#endif