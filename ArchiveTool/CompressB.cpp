#include "CompressB.h"
#include <string.h>

uint8_t* DecompressB(FileReader* input, uint32_t decompressedLength, uint32_t compressedEnd, const int32_t shiftAmount) {
	uint32_t currDecomp = 0;

	uint8_t* dcmp = new uint8_t[(decompressedLength + 3) & ~3]; // allocate data aligned to 4 bytes

	uint32_t shiftRegister = 0;
	uint32_t cumulative = 0;
	uint32_t currBlockPos = 1;

	// TODO: VALIDATE LENGTH

	while (currDecomp < decompressedLength) {
		uint8_t rawBlockSize = input->ReadUInt8();
		int32_t currBlockSize = ((rawBlockSize + 1) << 1) - 1;
		uint8_t* currBlock = new uint8_t[currBlockSize+1];
		for (uint32_t i = 1; i <= currBlockSize; ++i) {
			currBlock[i] = input->ReadUInt8();
		}
		currBlock[0] = rawBlockSize; // genuinely unsure if this is needed or the devs really just expected the array to start on an odd number
		uint32_t blockDecomp = 0;
		while (currDecomp < decompressedLength) {
			uint32_t currPack = input->ReadUInt32(); // implemented originally as a weird mis-aligned read
			for (uint32_t i = 0; i < 32; ++i) {
				uint8_t offs = currBlock[currBlockPos];
				currBlockPos &= ~1;
				currBlockPos += ((offs & 0x3F) + 1) << 1;
				currBlockPos += (currPack & 0x80000000) ? 1 : 0;
				offs <<= currPack & 0x80000000 ? 1 : 0;
				currPack <<= 1;
				if (offs & 0x80) {
					uint8_t currValue = currBlock[currBlockPos];
					cumulative = cumulative >> shiftAmount;
					cumulative |= currValue << (32-shiftAmount);
					currBlockPos = 1;
					shiftRegister += shiftAmount;
					currBlockPos = 1;
					if ((currDecomp - decompressedLength) < (shiftRegister >> 3)) {
						cumulative >>= 32 - shiftRegister;
						shiftRegister = 32;
					}
					if (shiftRegister == 32) {
						*(uint32_t*)(&dcmp[currDecomp]) = cumulative;
						shiftRegister = 0;
						cumulative = 0;
						currDecomp += 4;
						if (currDecomp >= decompressedLength) {
							delete[] currBlock;
							int a = input->GetPosition();
							return dcmp;
						}
					}
				}
			}
		}

		delete[] currBlock;
	}
	return dcmp;
}