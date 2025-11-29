#pragma once
#include <stdio.h>
#include <stdint.h>

class FileReader {
private:
	FILE *f;
public:
	FileReader(const char *file);
	~FileReader();

	bool IsValid();

	uint32_t GetLength();
	uint32_t GetPosition();
	void Seek(uint32_t position);

	uint8_t ReadUInt8();
	uint16_t ReadUInt16();
	uint32_t ReadUInt32();
	int8_t ReadInt8();
	int16_t ReadInt16();
	int32_t ReadInt32();
};