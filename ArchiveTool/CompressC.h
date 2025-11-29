#pragma once
#include <stdint.h>
#include "Reader.h"

uint8_t* DecompressC(FileReader* f, uint32_t decompressedSize, uint32_t compressedEnd);