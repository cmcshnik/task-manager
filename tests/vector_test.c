#include "graph_test.h"

START_TEST(test_intvector_simple1) {
    IntVector* vec = NewIntVector(5);

    AppendToIntVector(vec, 1);
    AppendToIntVector(vec, 2);
    AppendToIntVector(vec, 3);
    
    ck_assert(GetIntVectorLength(vec) == 3);
    FreeIntVector(vec);
} END_TEST

START_TEST(test_intvector_simple2) {
    IntVector* vec = NewIntVector(500);

    InitializeIntVector(vec, 123);

    ck_assert(GetIntVectorLength(vec) == 500);
    FreeIntVector(vec);
} END_TEST

START_TEST(test_intvector_simple3) {
    IntVector* vec = NewIntVector(500);

    InitializeIntVector(vec, 123);

    ck_assert(GetIntVectorElement(vec, 10) == 123);
    FreeIntVector(vec);
} END_TEST


START_TEST(test_intvector_simple4) {
    IntVector* vec = NewIntVector(500);

    InitializeIntVector(vec, 123);

    SetIntVectorElement(vec, 10, 5);
    ChangeIntVectorElement(vec, 10, 15);

    ck_assert(GetIntVectorElement(vec, 10) == 20);
    FreeIntVector(vec);
} END_TEST

START_TEST(test_intvector_large) {
    int n = 100000; // works with 10000000
    IntVector* vec = NewIntVector(n);

    InitializeIntVector(vec, n);

    ck_assert(GetIntVectorElement(vec, 10000) == n);
    FreeIntVector(vec);
} END_TEST




START_TEST(test_stringvector_simple1) {
    StringVector* vec = NewStringVector(5);

    AppendToStringVector(vec, "wefwef");
    AppendToStringVector(vec, "21fwe1f1f");
    AppendToStringVector(vec, "wefwefwefw");
    
    ck_assert(GetStringVectorLength(vec) == 3);
    FreeStringVector(vec);
} END_TEST

START_TEST(test_stringvector_simple2) {
    StringVector* vec = NewStringVector(500);

    for (int i = 0; i < 500; ++i) {
        AppendToStringVector(vec, "123");
    }

    ck_assert(GetStringVectorLength(vec) == 500);
    FreeStringVector(vec);
} END_TEST

START_TEST(test_stringvector_simple3) {
    StringVector* vec = NewStringVector(500);

    for (int i = 0; i < 500; ++i) {
        AppendToStringVector(vec, "123");
    }

    ck_assert(strcmp(GetStringVectorElement(vec, 10), "123") == 0);
    FreeStringVector(vec);
} END_TEST

START_TEST(test_stringvector_simple4) {
    StringVector* vec = NewStringVector(500);

    for (int i = 0; i < 500; ++i) {
        AppendToStringVector(vec, "123");
    }

    SetStringVectorElement(vec, 10, "5");

    ck_assert(strcmp(GetStringVectorElement(vec, 10), "5") == 0);
    FreeStringVector(vec);
} END_TEST

START_TEST(test_stringvector_deletions) {
    StringVector* vec = NewStringVector(3);


    AppendToStringVector(vec, "1");
    AppendToStringVector(vec, "2");
    AppendToStringVector(vec, "3");

    DeleteStringVectorElement(vec, 0);
    DeleteStringVectorElement(vec, 0);

    ck_assert(strcmp(GetStringVectorElement(vec, 0), "3") == 0);
    FreeStringVector(vec);
} END_TEST

START_TEST(test_stringvector_large) {
    int n = 100000; // works with 10000000 (with ~750 MB usage of RAM)
    char* str = "111111111111111111111111111111111111111111111111111111111111111111";
    StringVector* vec = NewStringVector(n);

    for (int i = 0; i < n; ++i) {
        AppendToStringVector(vec, str);
    }

    ck_assert(strcmp(GetStringVectorElement(vec, 10000), str) == 0);
    FreeStringVector(vec);
} END_TEST


Suite* make_vector_suite(void) {
    Suite *s = suite_create("Vector tests");
    TCase *tc;

    tc = tcase_create("IntVector");
    tcase_add_test(tc, test_intvector_simple1);
    tcase_add_test(tc, test_intvector_simple2);
    tcase_add_test(tc, test_intvector_simple3);
    tcase_add_test(tc, test_intvector_simple4);
    tcase_add_test(tc, test_intvector_large);
    

    suite_add_tcase(s, tc);

    tc = tcase_create("StringVector");
    tcase_add_test(tc, test_stringvector_simple1);
    tcase_add_test(tc, test_stringvector_simple2);
    tcase_add_test(tc, test_stringvector_simple3);
    tcase_add_test(tc, test_stringvector_simple4);
    tcase_add_test(tc, test_stringvector_large);
    tcase_add_test(tc, test_stringvector_deletions);

    suite_add_tcase(s, tc);

    return s;
}
