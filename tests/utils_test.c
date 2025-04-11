#include "utils_test.h"

START_TEST(test_split_simple) {
    char* str = "1 1 1 1 1 1 1 1 1 1 1 1 1 1";
    StringVector* vec = SplitString(str, " ");

    bool flag = 1;
    for (int i = 0; i < GetStringVectorLength(vec); ++i) {
        flag = flag & (strcmp(GetStringVectorElement(vec, i), "1") == 0);
    }

    ck_assert(flag);
    FreeStringVector(vec);
} END_TEST

START_TEST(test_split_empty) {
    char* str = "";
    StringVector* vec = SplitString(str, " ");

    ck_assert(GetStringVectorLength(vec) == 0);
    FreeStringVector(vec);
} END_TEST

START_TEST(test_split_single) {
    char* str = "1";
    StringVector* vec = SplitString(str, " ");

    ck_assert((GetStringVectorLength(vec) == 1) && (strcmp(GetStringVectorElement(vec, 0), "1") == 0));
    FreeStringVector(vec);
} END_TEST

START_TEST(test_glue_simple1) {
    StringVector* vec = NewStringVector(5);
    
    for (int i = 0; i < 5; ++i) {
        AppendToStringVector(vec, "1");
    }

    char* buf = GlueStringVectorWithDelimeter(vec, "");

    ck_assert(strcmp(buf, "11111") == 0);

    FreeStringVector(vec);
    free(buf);
} END_TEST

START_TEST(test_glue_simple2) {
    StringVector* vec = NewStringVector(5);
    
    for (int i = 0; i < 5; ++i) {
        AppendToStringVector(vec, "1");
    }

    char* buf = GlueStringVectorWithDelimeter(vec, "222");

    ck_assert(strcmp(buf, "12221222122212221") == 0);

    FreeStringVector(vec);
    free(buf);
} END_TEST

START_TEST(test_glue_empty) {
    StringVector* vec = NewStringVector(5);
    
    char* buf = GlueStringVectorWithDelimeter(vec, "222");

    ck_assert(strcmp(buf, "") == 0);

    FreeStringVector(vec);
    free(buf);
} END_TEST

START_TEST(test_join_path_simple1) {
    char* buf = JoinPath("bin/", "/bash");

    ck_assert(strcmp(buf, "bin/bash") == 0);

    free(buf);
} END_TEST

START_TEST(test_join_path_simple2) {
    char* buf = JoinPath("bin", "bash");

    ck_assert(strcmp(buf, "bin/bash") == 0);

    free(buf);
} END_TEST

Suite* make_utils_suite(void) {
    Suite *s = suite_create("Map tests");
    TCase *tc;

    tc = tcase_create("Split func tests");
    tcase_add_test(tc, test_split_simple);
    tcase_add_test(tc, test_split_single);
    tcase_add_test(tc, test_split_empty);

    suite_add_tcase(s, tc);

    tc = tcase_create("Glue func tests");
    tcase_add_test(tc, test_glue_simple1);
    tcase_add_test(tc, test_glue_simple2);
    tcase_add_test(tc, test_glue_empty);

    suite_add_tcase(s, tc);

    tc = tcase_create("Join path func tests");
    tcase_add_test(tc, test_join_path_simple1);
    tcase_add_test(tc, test_join_path_simple2);

    suite_add_tcase(s, tc);
    return s;
}
