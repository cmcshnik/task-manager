#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <stdio.h>

typedef struct IntVector {
    int *arr_;
    size_t len_;
    size_t capacity_;
} IntVector;

// Create new vector<int> instance.
// Returns NULL on error.
IntVector* NewIntVector(size_t capacity);

// Free vector<int> instance.
// Ignores NULL instance and fields.
void FreeIntVector(IntVector* vector);

// Fill vector elements (up to current capacity) with the provided value, adjusting length if needed.
void InitializeIntVector(IntVector* vector, int value);

// Append element to a vector, with pisibility to append NULL pointers.
// Returns true on success, otherwise returns false.
bool AppendToIntVector(IntVector* vector, int elem);

// Get vector length.
size_t GetIntVectorLength(const IntVector* vector);

// Get vector element by index.
int GetIntVectorElement(const IntVector* vector, size_t idx);

// Set vector element by index.
void SetIntVectorElement(IntVector* vector, size_t idx, int elem);

// Add delta to a vector element by index.
// Returns resulting element value.
int ChangeIntVectorElement(IntVector* vector, size_t idx, int delta);

// Reverse int vector
void ReverseIntVector(IntVector* vec);


typedef struct StringVector {
    char** arr_;
    size_t len_;
    size_t capacity_;
} StringVector;

// Create new vector<string> instance.
// Returns NULL on error.
StringVector* NewStringVector(size_t capacity);

// Free vector<string> instance.
// Ignores NULL instance and fields.
void FreeStringVector(StringVector* vector);

// Append element to avector.
// Returns true on success, otherwise returns false.
bool AppendToStringVector(StringVector* vector, const char* elem);

// Get vector length.
size_t GetStringVectorLength(const StringVector* vector);

// Get vector element by index.
const char* GetStringVectorElement(const StringVector* vector, size_t idx);

// Set vector element by index.
void SetStringVectorElement(StringVector* vector, size_t idx, const char* elem);

// Get raw pointer to the underlying string array.
char** GetStringVectorData(StringVector* vector);

// Delete element
void DeleteStringVectorElement(StringVector* vector, size_t idx);