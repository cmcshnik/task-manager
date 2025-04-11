#include "map.h"

const unsigned long Hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  
    }
    return hash;
}


StringMap* NewStringMap(size_t capacity) {
    StringMap* res = malloc(sizeof(StringMap));
    if (!res) {
        errno = ENOMEM;
        return NULL;
    }

    res->values_ = NewIntVector(capacity);
    if (!res->values_) {
        free(res);
        return NULL;
    }

    InitializeIntVector(res->values_, 0);

    res->keys_ = malloc(sizeof(char*) * capacity);
    if (!res->keys_) {
        FreeIntVector(res->values_);
        free(res);
        return NULL;
    }

    for (int i = 0; i < capacity; ++i) {
        res->keys_[i] = NULL;
    }

    res->len_ = 0;
    res->capacity_ = capacity;
      
    return res;
}

void FreeStringMap(StringMap* map) {
    if (map == NULL) {
        return;
    }

    for (int i = 0; i < map->capacity_; ++i) {
        free(map->keys_[i]);
    }

    free(map->keys_);
    FreeIntVector(map->values_);
    free(map);
}

// Linear colision solving
// Setting idx as found index if resolved, otherwise idx is undefined
// return values:
// -1 - error (table is full or etc)
//  0 - new key
//  1 - not new key
static int ActualStringMapIndex(const StringMap* map, const char* key, int* idx) {
    *idx = Hash(key) % map->capacity_;
    int count = 0;

    while (true) {
        if (count == map->capacity_) {
            return -1;
        }

        if (map->keys_[*idx] == NULL) {
            return 0;
        }

        if (strcmp(key, map->keys_[*idx]) == 0) {
            return 1;
        }

        *idx = (*idx + 1) % map->capacity_;
        count++;
    }
}

// Search for the key and fetch the corresponding value.
// Returns true (and sets the value) if the key is present, otherwise returns false.
bool GetStringMapValue(const StringMap* map, const char* key, int* value) {
    if (map == NULL || key == NULL || value == NULL) {
        errno = EINVAL;
        return false;
    }

    int idx;
    int status;

    status = ActualStringMapIndex(map, key, &idx);
    if (status == -1 || status == 0) {
        return false;
    } 

    int value_in_table = GetIntVectorElement(map->values_, idx);
    *value = value_in_table;
    return true;
}

// Set the value for the key.
// Returns true if the key wasn't present, otherwise returns false.
bool SetStringMapValue(StringMap* map, const char* key, int value, bool do_change_if_exists) {
    if (map == NULL || key == NULL) {
        errno = EINVAL;
        return false;
    }

    int idx;
    int status;

    status = ActualStringMapIndex(map, key, &idx);
    if (status == -1) {
        return false;
    } else if (status == 0) {
        map->keys_[idx] = strdup(key);
        if (!map->keys_[idx]) {
            errno = ENOMEM;
            return false;
        }

        SetIntVectorElement(map->values_, idx, value);
        map->len_++;
        return true;
    } else {
        if (do_change_if_exists) {
            SetIntVectorElement(map->values_, idx, value);
            return true;
        }

        return false;
    }
}


// Create new instance of map<int, int>.
// Returns NULL on error.
IntMap* NewIntMap(size_t capacity) {
    IntMap* res = NewStringMap(capacity);
    return res;
}

// Free map<int, int> instance.
// Ignores NULL instance or fields.
void FreeIntMap(IntMap* map) {
    FreeStringMap(map);
}


// Search for the key and fetch the corresponding value.
// Returns true (and sets the value) if the key is present, otherwise returns false.
bool GetIntMapValue(const IntMap* map, int key, int* value) {
    char string_key[12];
    snprintf(string_key, 12, "%d", key);
    return GetStringMapValue(map, string_key, value);
}

// Set the value for the key.
// Returns true if the key wasn't present, otherwise returns false.
bool SetIntMapValue(IntMap* map, int key, int value, bool do_change_if_exists) {
    char string_key[12];
    snprintf(string_key, 12, "%d", key);
    return SetStringMapValue(map, string_key, value, do_change_if_exists);
}

// Copy int map
// IntMap* CopyIntMap(const IntMap* map) {
//     IntMap* res = NewStringMap(map->capacity_);
//     if (!res) {
//         return NULL;
//     }

//     for (int i = 0; i < map->capacity_; ++i) {
//         if (map->keys_[i]) {
//             res->keys_[i] = strdup(map->keys_[i]);

//             if (!res->keys_[i]) {
//                 errno = ENOMEM;
//                 FreeIntMap(res);
//                 return NULL;
//             }
//         }

//         SetIntVectorElement(res->values_, i, GetIntVectorElement(map->values_, i));
//     }
    
//     return res;
// }

