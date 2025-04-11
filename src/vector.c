#include "vector.h"

IntVector* NewIntVector(size_t capacity) {
    IntVector* vec = malloc(sizeof(IntVector));
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }

    vec->arr_ = malloc(sizeof(int) * capacity);
    if (!vec->arr_) {
        free(vec);
        errno = ENOMEM;
        return NULL;
    }

    vec->capacity_ = capacity;
    vec->len_ = 0;

    return vec;
}

void FreeIntVector(IntVector* vector) {
    if (vector == NULL) {
        return;
    }

    free(vector->arr_);
    free(vector);
}

void InitializeIntVector(IntVector* vector, int value) { 
    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return;
    }

    for (int i = vector->len_; i < vector->capacity_; ++i) {
        vector->arr_[i] = value;
    }

    vector->len_ = vector->capacity_;
}

bool AppendToIntVector(IntVector* vector, int elem) {
    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector->len_ == vector->capacity_) {
        if (vector->capacity_ == 0) {
            vector->capacity_ = 1;
        } else {
            vector->capacity_ *= 2;
        }

        vector->arr_ = realloc(vector->arr_, sizeof(int) * vector->capacity_);
        if (!vector->arr_) {
            errno = ENOMEM;
            return false;
        }        
    }

    vector->arr_[vector->len_] = elem;
    vector->len_++;

    return true;
}

size_t GetIntVectorLength(const IntVector* vector) {
    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->len_;
}

int GetIntVectorElement(const IntVector* vector, size_t idx) {
    if (idx >= vector->len_) {
        errno = ERANGE;
        return 0;
    }

    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->arr_[idx];
}

void SetIntVectorElement(IntVector* vector, size_t idx, int elem) {
    if (idx >= vector->len_) {
        errno = ERANGE;
        return;
    }

    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return;
    }

    vector->arr_[idx] = elem;
}

int ChangeIntVectorElement(IntVector* vector, size_t idx, int delta) {
    if (idx >= vector->len_) {
        errno = ERANGE;
        return 0;
    }

    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->arr_[idx] += delta;
}

void ReverseIntVector(IntVector* vec) {
    if (!vec) {
        return;
    }

    int buf;
    for (int i = 0; i < GetIntVectorLength(vec) / 2; ++i) {
        buf = GetIntVectorElement(vec, GetIntVectorLength(vec) - i - 1);
        SetIntVectorElement(vec, GetIntVectorLength(vec) - i - 1, GetIntVectorElement(vec, i));
        SetIntVectorElement(vec, i, buf);
    }
}






StringVector* NewStringVector(size_t capacity) {
    StringVector* vec = malloc(sizeof(StringVector));
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }

    vec->arr_ = malloc(sizeof(char*) * capacity);
    if (!vec->arr_) {
        errno = ENOMEM;
        return NULL;
    }

    vec->capacity_ = capacity;
    vec->len_ = 0;

    return vec;
}

void FreeStringVector(StringVector* vector) {
    if (vector == NULL) {
        return;
    }

    if (vector->arr_ == NULL) {
        free(vector);
        return;
    }

    for (int i = 0; i < vector->len_; ++i) {
        free(vector->arr_[i]);
    }

    free(vector->arr_);
    free(vector);
}

bool AppendToStringVector(StringVector* vector, const char* elem) {
    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector->capacity_ == vector->len_) {
        if (vector->capacity_ == 0) {
            vector->capacity_ = 1;
        } else {
            vector->capacity_ *= 2;
        }

        vector->arr_ = realloc(vector->arr_, sizeof(char*) * vector->capacity_);
        if (!vector->arr_) {
            errno = ENOMEM;
            return false;
        }
    }

    if (elem != NULL) {
        vector->arr_[vector->len_] = strdup(elem);
        if (!vector->arr_[vector->len_]) {
            errno = ENOMEM;
            return false;
        }
    } else {
        vector->arr_[vector->len_] = (char*)NULL;
    }

    vector->len_++;
    return true;
}

size_t GetStringVectorLength(const StringVector* vector) {
    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->len_;
}

const char* GetStringVectorElement(const StringVector* vector, size_t idx) {
    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return "";
    }

    if (idx >= vector->len_) {
        errno = ERANGE;
        return "";
    }

    return vector->arr_[idx];
}

void SetStringVectorElement(StringVector* vector, size_t idx, const char* elem) {
    if (vector == NULL || vector->arr_ == NULL || elem == NULL) {
        errno = EINVAL;
        return;
    }

    if (idx >= GetStringVectorLength(vector)) {
        errno = ERANGE;
        return;
    }

    free(vector->arr_[idx]);

    vector->arr_[idx] = strdup(elem);
    if (!vector->arr_[idx]) {
        errno = ENOMEM;
        return;
    }
}

char** GetStringVectorData(StringVector* vector) {
    if (vector == NULL || vector->arr_ == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return vector->arr_;
}

void DeleteStringVectorElement(StringVector* vector, size_t idx) {
    if (vector == NULL) {
        errno = EINVAL;
        return;
    }

    if (idx >= GetStringVectorLength(vector)) {
        errno = ERANGE;
        return;
    }

    free(vector->arr_[idx]);

    for (int i = idx; i < GetStringVectorLength(vector) - 1; ++i) {
        vector->arr_[i] = vector->arr_[i + 1];
    }

    vector->arr_[GetStringVectorLength(vector) - 1] = NULL;
    vector->len_--;
}
