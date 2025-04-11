#include "graph_test.h"

START_TEST(test_graph_is_acyclic_simple1) {
    Graph* g = NewGraph(10);
    ck_assert_ptr_nonnull(g);

    AddDirectedEdge(g, 0, 5);
    AddDirectedEdge(g, 5, 1);
    AddDirectedEdge(g, 1, 4);
    AddDirectedEdge(g, 4, 0);

    ck_assert(!IsAcyclic(g));
    FreeGraph(g);
} END_TEST

START_TEST(test_graph_is_acyclic_simple2) {
    Graph* g = NewGraph(10);
    ck_assert_ptr_nonnull(g);

    AddDirectedEdge(g, 0, 5);
    AddDirectedEdge(g, 5, 1);
    AddDirectedEdge(g, 1, 4);
    AddDirectedEdge(g, 4, 6);

    ck_assert(IsAcyclic(g));
    FreeGraph(g);
} END_TEST

START_TEST(test_graph_is_acyclic_multiple_edges) {
    Graph* g = NewGraph(10);
    ck_assert_ptr_nonnull(g);

    for (int i = 0; i < 100; ++i) {
        AddDirectedEdge(g, 0, 5);
    }

    ck_assert(IsAcyclic(g));
    FreeGraph(g);
} END_TEST

START_TEST(test_graph_is_acyclic_single_loop) {
    Graph* g = NewGraph(1);
    ck_assert_ptr_nonnull(g);

    AddDirectedEdge(g, 0, 0);
    
    ck_assert(!IsAcyclic(g));
    FreeGraph(g);
} END_TEST

START_TEST(test_graph_is_acyclic_many_loops) {
    Graph* g = NewGraph(1000);
    ck_assert_ptr_nonnull(g);

    for (int i = 0; i < 1000; ++i) {
        AddDirectedEdge(g, i, i);
    }

    ck_assert(!IsAcyclic(g));
    FreeGraph(g);
} END_TEST

START_TEST(test_graph_size) {
    Graph* g = NewGraph(5);
    ck_assert_ptr_nonnull(g);

    ck_assert(GetGraphSize(g) == 5);
    FreeGraph(g);
} END_TEST

START_TEST(test_graph_is_acyclic_very_large_loop) {
    int vertices_count = 10000;
    Graph* g = NewGraph(vertices_count);
    ck_assert_ptr_nonnull(g);

    for (int i = 0; i < vertices_count - 1; ++i) {
        AddDirectedEdge(g, i, i+1);
    }

    AddDirectedEdge(g, vertices_count - 1, 0);

    ck_assert(!IsAcyclic(g));
    FreeGraph(g);
} END_TEST

START_TEST(test_graph_is_acyclic_very_large_chain) {
    int vertices_count = 10000;
    Graph* g = NewGraph(vertices_count);
    ck_assert_ptr_nonnull(g);

    for (int i = 0; i < vertices_count - 1; ++i) {
        AddDirectedEdge(g, i, i+1);
    }

    ck_assert(IsAcyclic(g));
    FreeGraph(g);
} END_TEST

Suite* make_graph_is_acyclic_suite(void) {
    Suite *s = suite_create("Graph::IsAcyclic");
    TCase *tc;

    tc = tcase_create("SimpleTests");
    tcase_add_test(tc, test_graph_is_acyclic_simple1);
    tcase_add_test(tc, test_graph_is_acyclic_simple2);
    tcase_add_test(tc, test_graph_size);
    tcase_add_test(tc, test_graph_is_acyclic_single_loop);
    suite_add_tcase(s, tc);

    tc = tcase_create("StressTests");
    tcase_add_test(tc, test_graph_is_acyclic_multiple_edges);
    tcase_add_test(tc, test_graph_is_acyclic_many_loops);
    tcase_add_test(tc, test_graph_is_acyclic_very_large_loop);
    tcase_add_test(tc, test_graph_is_acyclic_very_large_chain);
    suite_add_tcase(s, tc);

    return s;
}
