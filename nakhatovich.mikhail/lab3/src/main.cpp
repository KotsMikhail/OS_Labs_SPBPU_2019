#include <stdio.h>

#include "config.h"
#include "test.h"
#include "utils.h"

void load_config(int argc, char **argv)
{
    init_home_directory();
    config_t * config = nullptr;
    if (argc > 1)
        config = config_t::get_instance(argv[1]);
    if (!config || !config->load())
    {
        printf("Couldn't initialize configuration file.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    load_config(argc, argv);

    printf("\nFine-Grained set\n");
    run_simple_test(SET_FINE, TEST_WRITERS);
    run_simple_test(SET_FINE, TEST_READERS);
    run_simple_test(SET_FINE, TEST_COMMON);

    printf("\nOptimistic set\n");
    run_simple_test(SET_OPTIMISTIC, TEST_WRITERS);
    run_simple_test(SET_OPTIMISTIC, TEST_READERS);
    run_simple_test(SET_OPTIMISTIC, TEST_COMMON);

    printf("\n");
    run_time_test();

    return 0;
}
