#include "CompressC.h"

uint8_t* DecompressC(FileReader* f, uint32_t decompressedSize, uint32_t compressedEnd) {
	// simple RLE
	uint8_t* dcmp = new uint8_t[decompressedSize];
	uint32_t dcmpPos = 0;

	while (dcmpPos < decompressedSize && f->GetPosition() < (compressedEnd-1)) {
		uint8_t controlChar = f->ReadUInt8();
		if ((controlChar & 0x80) == 0) {
			for (uint32_t i = 0; i <= (controlChar & 0x7F); ++i) {
				dcmp[dcmpPos] = f->ReadUInt8();
				++dcmpPos;
			}
		}
		else {
			uint32_t copyCount = controlChar & 0x7F;
			copyCount += 2;
			uint8_t toCopy = f->ReadUInt8();
			for (uint32_t i = 0; i <= copyCount; ++i) {
				dcmp[dcmpPos] = toCopy;
				++dcmpPos;
			}
		}
	}

	return dcmp;
}