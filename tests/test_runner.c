#include <stdlib.h>

#include <check.h>

#include "graph_test.h"
#include "map_test.h"
#include "queue_test.h"
#include "vector_test.h"
#include "utils_test.h"
#include "config_test.h"

int main(void) {
    SRunner *runner = srunner_create(NULL);
    srunner_add_suite(runner, make_graph_is_acyclic_suite());
    srunner_add_suite(runner, make_map_suite());
    srunner_add_suite(runner, make_queue_suite());
    srunner_add_suite(runner, make_vector_suite());
    srunner_add_suite(runner, make_utils_suite());
    srunner_add_suite(runner, make_config_suite());
    // TODO:
    // * graph tests
    // * map tests
    // * queue tests
    // * vector tests
    // * utils tests

    srunner_run_all(runner, CK_VERBOSE);
    int num_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
