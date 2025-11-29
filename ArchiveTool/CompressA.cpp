#include "CompressA.h"

uint8_t* DecompressA(FileReader* f, uint32_t decompressedSize, uint32_t compressedEnd) {
    uint8_t* dcmp = new uint8_t[decompressedSize]();
    uint32_t dcmpSize = 0;
    uint32_t controlByte;
    uint8_t controlByteBits = 0;
    uint32_t copyBackControl = 0; // packed control character indicating how many to copy, how far back to copy, etc
    uint32_t copyBackByteCount = 3; // how many bytes to read for the copy back control; 1-3
    uint8_t compressionType = 0; // if set to 0, only 1 byte will ever be read for copy length/distance
    // maybe set to 1 based on first 3 bits of flags value; either invert bit 0 as i see 1 set on my examples, or bit 1. bit 2 seems to indicate a completely different compression
    uint8_t* ret = dcmp;
    for (;;) {
        if (decompressedSize <= 0) {
            break;
        }
        for (;;) {
            if (controlByteBits == 0) {
                break;
            }
            if (compressedEnd - f->GetPosition() <= 0) {
                goto FINISH_COMPRESSA;
            }
            if ((controlByte & 0x80) == 0) {
                *dcmp = f->ReadUInt8();
                --decompressedSize;
                ++dcmp;
                ++dcmpSize;
            }
            else {
                int32_t copyBackDistance;
                for (;;) {
                    if (copyBackByteCount == 0) {
                        goto BREAKCOMPRESSATYPE1;
                    }
                    if (compressionType == 1) { // ? this value is never written to? is it a versioning indicator?
                        --copyBackByteCount;
                        if (copyBackByteCount != 0) {
                            if (copyBackByteCount != 1) {
                                int extraToAdd; // this value is mildly confusing; wanting to copy 0x100 bytes is actually
                                // pretty reasonable, but you can't with this system?
                                copyBackControl = f->ReadUInt8();
                                if (copyBackControl & 0xE0) { // upper 3 bits are set means only 1 byte; add 1 since you wouldn't compress a single byte
                                    copyBackControl += 0x10;
                                    copyBackByteCount = 0;
                                    goto BYTE_READ_COMPRESSA;
                                }
                                else {
                                    extraToAdd = 0x110;
                                    if ((copyBackControl & 0x10) != 0) { // mark as 1 byte to copy back to indicate that it's actually encoded in 3 bytes, as 1 byte is unreasonable to compress
                                        extraToAdd += 0x1000;
                                        copyBackControl = extraToAdd + ((copyBackControl & 0xF) << 16);
                                    }
                                    else { // no bytes to copy back means it's encoded in 2 bytes
                                        copyBackControl = extraToAdd + ((copyBackControl & 0xF) << 8);
                                        copyBackByteCount = 1;
                                    }
                                }
                            }
                            else {
                                copyBackControl += f->ReadUInt8() << 8;
                            }
                        }
                        else {
                            copyBackControl += f->ReadUInt8();
                            goto BYTE_READ_COMPRESSA;
                        }

                    }
                    else {
                        break;
                    }
                    if (!(compressedEnd - f->GetPosition() <= 0)) {
                        continue;
                    }
                    goto FINISH_COMPRESSA;
                }
                copyBackControl = 0x30 + f->ReadUInt8();
                copyBackByteCount = 0;

            BYTE_READ_COMPRESSA:
                if (compressedEnd - f->GetPosition() <= 0) {
                    goto FINISH_COMPRESSA;
                }
            BREAKCOMPRESSATYPE1:
                copyBackDistance = 1 + (((copyBackControl & 0xF) << 8) | f->ReadUInt8());
                copyBackByteCount = 3;


                copyBackControl >>= 4;
                if (copyBackControl != 0) { // why isn't this just a while?
                    do {
                        *dcmp = dcmp[-copyBackDistance];
                        --decompressedSize;
                        ++dcmp;
                        ++dcmpSize;
                    } while (--copyBackControl > 0);
                }
            }
            if (decompressedSize == 0) {
                goto FINISH_COMPRESSA;
            }
            controlByte <<= 1;
            --controlByteBits;
        }
        if (compressedEnd - f->GetPosition() <= 0) break;
        controlByte = f->ReadUInt8();
        controlByteBits = 8;
    }
FINISH_COMPRESSA:
    int a = f->GetPosition();
    return ret;
}