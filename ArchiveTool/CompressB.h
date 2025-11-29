#pragma once
#include <stdint.h>
#include "Reader.h"

uint8_t* DecompressB(FileReader* input, uint32_t decompressedLength, uint32_t compressedEnd, const int32_t shiftAmount);