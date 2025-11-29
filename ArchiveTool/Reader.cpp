#include "Reader.h"

FileReader::FileReader(const char* file) {
	f = fopen(file, "rb");
}

FileReader::~FileReader() {
	if (f == NULL) {
		return;
	}
	fclose(f);
}

bool FileReader::IsValid() {
	return f != NULL;
}

uint32_t FileReader::GetLength() {
	uint32_t currPos = ftell(f);
	fseek(f, 0, SEEK_END);
	uint32_t length = ftell(f);
	fseek(f, currPos, SEEK_SET);
	return length;
}

uint32_t FileReader::GetPosition() {
	return ftell(f);
}

void FileReader::Seek(uint32_t position) {
	fseek(f, position, SEEK_SET);
}

uint8_t FileReader::ReadUInt8() {
	uint8_t ret;
	fread(&ret, 1, 1, f);
	return ret;
}

uint16_t FileReader::ReadUInt16() {
	uint16_t ret;
	fread(&ret, 2, 1, f);
	return ret;
}

uint32_t FileReader::ReadUInt32() {
	uint32_t ret;
	fread(&ret, 4, 1, f);
	return ret;
}

int8_t FileReader::ReadInt8() {
	int8_t ret;
	fread(&ret, 1, 1, f);
	return ret;
}

int16_t FileReader::ReadInt16() {
	int16_t ret;
	fread(&ret, 2, 1, f);
	return ret;
}

int32_t FileReader::ReadInt32() {
	int32_t ret;
	fread(&ret, 4, 1, f);
	return ret;
}