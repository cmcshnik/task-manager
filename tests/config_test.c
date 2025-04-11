#include "config_test.h"

START_TEST(test_config_empty) {
    FILE* file = fopen("./tests/config_folder/bad1.cfg", "r");
    ExecutionConfig* config = ReadExecutionConfig(file, ".");

    ck_assert(config->num_tasks == 0);
    fclose(file);
    FreeExecutionConfig(config);
} END_TEST

START_TEST(test_config_bad2) {
    FILE* file = fopen("./tests/config_folder/bad2.cfg", "r");

    ExecutionConfig* config = ReadExecutionConfig(file, ".");

    ck_assert(config == NULL);
    fclose(file);
} END_TEST

START_TEST(test_config_bad3) {
    FILE* file = fopen("./tests/config_folder/bad3.cfg", "r");
    ExecutionConfig* config = ReadExecutionConfig(file, ".");

    ck_assert(config == NULL);
    fclose(file);
} END_TEST

START_TEST(test_config_bad4) {
    FILE* file = fopen("./tests/config_folder/bad4.cfg", "r");
    ExecutionConfig* config = ReadExecutionConfig(file, ".");

    ck_assert(config == NULL);
    fclose(file);
} END_TEST

START_TEST(test_config_bad5) {
    FILE* file = fopen("./tests/config_folder/bad5.cfg", "r");
    ExecutionConfig* config = ReadExecutionConfig(file, ".");

    ck_assert(config == NULL);
    fclose(file);
} END_TEST

START_TEST(test_config_bad6) {
    FILE* file = fopen("./tests/config_folder/bad6.cfg", "r");
    ExecutionConfig* config = ReadExecutionConfig(file, ".");

    ck_assert(config == NULL);
    fclose(file);
} END_TEST

START_TEST(test_config_bad7) {
    FILE* file = fopen("./tests/config_folder/bad7.cfg", "r");
    ExecutionConfig* config = ReadExecutionConfig(file, ".");

    ck_assert(config == NULL);
    fclose(file);
} END_TEST

START_TEST(test_config_good) {
    FILE* file = fopen("./tests/config_folder/normal.cfg", "r");
    ExecutionConfig* config = ReadExecutionConfig(file, ".");

    ck_assert(config != NULL);
    fclose(file);
    FreeExecutionConfig(config);
} END_TEST


Suite* make_config_suite(void) {
    Suite *s = suite_create("Graph::IsAcyclic");
    TCase *tc;

    tc = tcase_create("SimpleTests");
    tcase_add_test(tc, test_config_empty);
    tcase_add_test(tc, test_config_bad2);
    tcase_add_test(tc, test_config_bad3);
    tcase_add_test(tc, test_config_bad4);
    tcase_add_test(tc, test_config_bad5);
    tcase_add_test(tc, test_config_bad6);
    tcase_add_test(tc, test_config_bad7);
    tcase_add_test(tc, test_config_good);
    suite_add_tcase(s, tc);

    return s;
}
