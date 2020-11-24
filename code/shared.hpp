#ifndef SHARED_HPP
#define SHARED_HPP

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <ctype.h>

#include <set>
#include <map>
#include <vector>

#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t
#define sptr int64_t

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define uptr uint64_t

#define MINBUFFER 32
#define EOZ (-1) // End of stream
#define ARRAY_LEN(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

struct Buffer {
	void* buffer;
	void* old_buffer;
	uptr length;

	int writeIndex;
};

struct File {
	File();
	~File();

	bool Open(const char* filePath, const char* mode);
	bool Close();
	bool IsValid();
	bool Read(void* data, size_t bytes);
	bool Write(const void* data, size_t bytes);

	void* file_buffer;
};

bool ReadEntireFile(Buffer& buffer, const char* filePath);
bool AllocBuffer(Buffer& buffer, uptr bytes);

void PrintDebug(const char* format, ...);
void PrintError(const char* format, ...);
void FatalError(const char* format, ...);

#endif