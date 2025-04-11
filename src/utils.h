#pragma once

#include <stdarg.h>

#include "vector.h"
#include "queue.h"
#include "map.h"
#include "graph.h"

// Split string by delimiter.
// Empty parts are ignored.
// Returns NULL on error.
StringVector* SplitString(const char* str, const char* delim);

// Gluing all the elements of a vector into one large string
char* GlueStringVectorWithDelimeter(const StringVector* vec, const char* delim);

// Join two path pieces.
// Dots and slashes on the join boundary get squashed.
// Returns NULL on error.
char* JoinPath(const char* prefix, const char* suffix);

// Check whether the specified string str is the correct positive number and return it, 
// otherwise return -1
unsigned int MyAtoi(const char* str);