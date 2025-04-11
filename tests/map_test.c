#include "graph_test.h"

START_TEST(test_intmap_simple1) {
    IntMap* map = NewIntMap(5);
    ck_assert_ptr_nonnull(map);

    SetIntMapValue(map, 0, 1, true);
    SetIntMapValue(map, 1, 2, true);
    SetIntMapValue(map, 2, 3, true);

    int n1, n2, n3;

    GetIntMapValue(map, 0, &n1);
    GetIntMapValue(map, 1, &n2);
    GetIntMapValue(map, 2, &n3);

    ck_assert((n1 == 1) && (n2 == 2) && (n3 == 3));
    FreeIntMap(map);
} END_TEST

START_TEST(test_intmap_simple2) {
    IntMap* map = NewIntMap(5);
    ck_assert_ptr_nonnull(map);

    SetIntMapValue(map, -123, 1, true);
    SetIntMapValue(map, 11234, 2, true);
    SetIntMapValue(map, 141412, 3, true);

    int n1, n2, n3;

    GetIntMapValue(map, -123, &n1);
    GetIntMapValue(map, 11234, &n2);
    GetIntMapValue(map, 141412, &n3);

    ck_assert((n1 == 1) && (n2 == 2) && (n3 == 3));
    FreeIntMap(map);
} END_TEST

START_TEST(test_intmap_full_capacity) {
    int capacity = 10000;

    IntMap* map = NewIntMap(capacity);
    ck_assert_ptr_nonnull(map);

    for (int i = 0; i < capacity; ++i) {
        SetIntMapValue(map, i, i, true);
    }

    int value;
    bool res = 1;

    for (int i = 0; i < capacity; ++i) {
        GetIntMapValue(map, i, &value);
        res = res * (value == i);
    }

    ck_assert(res);
    FreeIntMap(map);
} END_TEST





START_TEST(test_stringmap_simple1) {
    StringMap* map = NewStringMap(5);
    ck_assert_ptr_nonnull(map);

    SetStringMapValue(map, "a", 1, true);
    SetStringMapValue(map, "b", 2, true);
    SetStringMapValue(map, "c", 3, true);

    int n1, n2, n3;

    GetStringMapValue(map, "a", &n1);
    GetStringMapValue(map, "b", &n2);
    GetStringMapValue(map, "c", &n3);

    ck_assert((n1 == 1) && (n2 == 2) && (n3 == 3));
    FreeStringMap(map);
} END_TEST

START_TEST(test_stringmap_simple2) {
    StringMap* map = NewStringMap(5);
    ck_assert_ptr_nonnull(map);

    SetStringMapValue(map, "wefweuifwieurgfwiegfywieurgwiuergywerguwegrfyiewugfwuegrfy", 1, true);
    SetStringMapValue(map, "1", 2, true);
    SetStringMapValue(map, "000000000000000000000000000000000000", 3, true);

    int n1, n2, n3;

    GetStringMapValue(map, "wefweuifwieurgfwiegfywieurgwiuergywerguwegrfyiewugfwuegrfy", &n1);
    GetStringMapValue(map, "1", &n2);
    GetStringMapValue(map, "000000000000000000000000000000000000", &n3);

    ck_assert((n1 == 1) && (n2 == 2) && (n3 == 3));
    FreeIntMap(map);
} END_TEST

START_TEST(test_stringmap_full_capacity_with_same_key_and_change) {
    int capacity = 10000;

    StringMap* map = NewStringMap(capacity);
    ck_assert_ptr_nonnull(map);

    for (int i = 0; i < capacity; ++i) {
        SetStringMapValue(map, "123", i, true);
    }

    int value;
    GetStringMapValue(map, "123", &value);

    ck_assert(value == capacity - 1);
    FreeStringMap(map);
} END_TEST

START_TEST(test_stringmap_full_capacity_with_same_key_and_no_change) {
    int capacity = 10000;

    StringMap* map = NewStringMap(capacity);
    ck_assert_ptr_nonnull(map);

    for (int i = 0; i < capacity; ++i) {
        SetStringMapValue(map, "123", i, false);
    }

    int value;
    GetStringMapValue(map, "123", &value);

    ck_assert(value == 0);
    FreeStringMap(map);
} END_TEST


Suite* make_map_suite(void) {
    Suite *s = suite_create("Map tests");
    TCase *tc;

    tc = tcase_create("IntMapTests");
    tcase_add_test(tc, test_intmap_simple1);
    tcase_add_test(tc, test_intmap_simple2);
    tcase_add_test(tc, test_intmap_full_capacity);

    suite_add_tcase(s, tc);

    tc = tcase_create("StringMapTests");
    tcase_add_test(tc, test_stringmap_simple1);
    tcase_add_test(tc, test_stringmap_simple2);
    tcase_add_test(tc, test_stringmap_full_capacity_with_same_key_and_change);
    tcase_add_test(tc, test_stringmap_full_capacity_with_same_key_and_no_change);

    suite_add_tcase(s, tc);

    return s;
}
