#include "graph_test.h"

START_TEST(test_queue_simple1) {
    Queue* q = NewQueue();
    ck_assert_ptr_nonnull(q);

    Push(q, 1);
    Push(q, 2);
    Push(q, 3);

    int n1, n2, n3;

    Front(q, &n1);
    Pop(q);
    Front(q, &n2);
    Pop(q);
    Front(q, &n3);
    Pop(q);

    ck_assert((n1 == 1) && (n2 == 2) && (n3 == 3));
    FreeQueue(q);
} END_TEST

START_TEST(test_queue_simple2) {
    Queue* q = NewQueue();
    ck_assert_ptr_nonnull(q);

    Push(q, 1);

    int n1;

    Front(q, &n1);
    Pop(q);
    Pop(q);
    Pop(q);
    Pop(q);

    ck_assert((n1 == 1));
    FreeQueue(q);
} END_TEST

START_TEST(test_queue_empty) {
    Queue* q = NewQueue();
    ck_assert_ptr_nonnull(q);

    Pop(q);
    Pop(q);
    Pop(q);
    Pop(q);

    ck_assert(IsEmpty(q));
    FreeQueue(q);
} END_TEST

START_TEST(test_queue_large_test) {
    int n = 100000; // works with 10000000

    Queue* q = NewQueue();
    ck_assert_ptr_nonnull(q);

    for (int i = 0; i < n; ++i) {
        Push(q, i);
    }

    bool res = 1;
    int value;
    for (int i = 0; i < n; ++i) {
        Front(q, &value);
        res = res * (value == i);
        Pop(q);
    }

    ck_assert(res);
    FreeQueue(q);
} END_TEST


Suite* make_queue_suite(void) {
    Suite *s = suite_create("Queue tests");
    TCase *tc;

    tc = tcase_create("QueueTests");
    tcase_add_test(tc, test_queue_simple1);
    tcase_add_test(tc, test_queue_simple2);
    tcase_add_test(tc, test_queue_large_test);
    tcase_add_test(tc, test_queue_empty);

    suite_add_tcase(s, tc);

    return s;
}
