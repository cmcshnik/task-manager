#include "utils.h"

// Split string by delimiter.
// Empty parts are ignored.
// Returns NULL on error.
StringVector* SplitString(const char* str, const char* delim) {
    StringVector* res = NewStringVector(2);
    if (!res) {
        return NULL;
    }

    char* str_copy = strdup(str);
    if (!str_copy) {
        FreeStringVector(res);
        errno = ENOMEM;
        return NULL;
    }

    bool status;
    char *token = strtok(str_copy, delim); 
    while (token != NULL) {
        if (token[strlen(token) - 1] == '\n') {
            token[strlen(token) - 1] = '\0';
        }
        
        status = AppendToStringVector(res, token);
        if (!status) {
            FreeStringVector(res);
            free(str_copy);
            return NULL;
        }

        token = strtok(NULL, delim);
    }

    if (strcmp(GetStringVectorElement(res, GetStringVectorLength(res) - 1), "") == 0) {
        DeleteStringVectorElement(res, GetStringVectorLength(res) - 1);
    }

    free(str_copy);
    return res;
}

char* GlueStringVectorWithDelimeter(const StringVector* vec, const char* delim) {
    size_t general_size = 0;
    char* res;

    if (GetStringVectorLength(vec) == 0) {
        return strdup("");
    }

    for (int i = 0; i < GetStringVectorLength(vec); ++i) {
        general_size += strlen(GetStringVectorElement(vec, i)) + strlen(delim);
    }

    res = malloc(sizeof(char) * (general_size - strlen(delim) + 1));
    if (!res) {
        errno = ENOMEM;
        return NULL;
    }

    res[0] = '\0';

    for (int i = 0; i < GetStringVectorLength(vec); ++i) {
        strcat(res, GetStringVectorElement(vec, i));

        if (i != (GetStringVectorLength(vec) - 1)) {
            strcat(res, delim);
        }
    }

    return res;
}

// Join two path pieces.
// Dots and slashes on the join boundary get squashed.
// Returns NULL on error.
char* JoinPath(const char* prefix, const char* suffix) {
    if (!prefix || !suffix) {
        errno = EINVAL;
        return NULL;
    }

    char* prefix_copy = strdup(prefix);
    if (!prefix_copy) {
        errno = EINVAL;
        return NULL;
    } 

    char* suffix_copy = strdup(suffix);
    if (!suffix_copy) {
        free(prefix_copy);
        errno = ENOMEM;
        return NULL;
    }

    char* suffix_copy2 = NULL;

    if (prefix_copy[strlen(prefix_copy) - 1] == '/' || prefix_copy[strlen(prefix_copy) - 1] == '.') {
        prefix_copy[strlen(prefix_copy) - 1] = '\0';
    }

    if (suffix_copy[0] == '/' || suffix_copy[0] == '.') {
        suffix_copy2 = suffix_copy;
        suffix_copy++;
    }

    char* res = malloc(sizeof(char) * (strlen(prefix_copy) + 1 + strlen(suffix_copy) + 1));
    if (!res) {
        free(prefix_copy);
        free(suffix_copy);
        errno = ENOMEM;
        return NULL;
    }

    res[0] = '\0';
    strcat(res, prefix_copy);
    strcat(res, "/");
    strcat(res, suffix_copy);

    free(prefix_copy);
    if (suffix_copy2) {
        free(suffix_copy2);
        suffix_copy = NULL;
    } else {
        free(suffix_copy);
    }

    return res;
}

unsigned int MyAtoi(const char* str) {
    if (!str) {
        return -1;
    }

    for (int i = 0; i < strlen(str); ++i) {
        if ((str[i] < '0') || ('9' < str[i])) {
            return -1;
        }
    }

    return (unsigned int)strtol(str, NULL, 10);
}