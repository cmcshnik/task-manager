#pragma once

#include "vector.h"

typedef struct StringMap {
    size_t capacity_;
    size_t len_;
    IntVector* values_;
    char** keys_;
} StringMap, IntMap;

// Create new instance of map<string, int>.
// Returns NULL on error.
StringMap* NewStringMap(size_t capacity);

// Free map<string, int> instance.
// Ignores NULL instance or NULL fields.
void FreeStringMap(StringMap* map);

// Search for the key and fetch the corresponding value.
// Returns true (and sets the value) if the key is present, otherwise returns false.
bool GetStringMapValue(const StringMap* map, const char* key, int* value);

// Set the value for the key.
// Returns true if the key wasn't present, otherwise returns false.
bool SetStringMapValue(StringMap* map, const char* key, int value, bool do_change_if_exists);


// Create new instance of map<int, int>.
// Returns NULL on error.
IntMap* NewIntMap(size_t capacity);

// Free map<int, int> instance.
// Ignores NULL instance or fields.
void FreeIntMap(IntMap* map);

// Search for the key and fetch the corresponding value.
// Returns true (and sets the value) if the key is present, otherwise returns false.
bool GetIntMapValue(const IntMap* map, int key, int* value);

// Set the value for the key.
// Returns true if the key wasn't present, otherwise returns false.
bool SetIntMapValue(IntMap* map, int key, int value, bool do_change_if_exists);

