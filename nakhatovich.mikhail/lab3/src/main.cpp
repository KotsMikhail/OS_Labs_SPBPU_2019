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
    if (!config || !(config->load()))
    {
        printf("Couldn't initialize configuration file.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    load_config(argc, argv);
    config_t * config = config_t::get_instance();

    printf("\nFine-Grained set\n");
    run_writers_test(SET_FINE, config->get_value(WRITERS), config->get_cnt_elements(WRITERS));
    run_readers_test(SET_FINE, config->get_value(READERS), config->get_cnt_elements(READERS));
    run_common_test(SET_FINE, config->get_value(COMMON_READERS), config->get_value(COMMON_WRITERS), config->get_value(COMMON_N));
    
    printf("\nOptimistic set\n");
    run_writers_test(SET_OPTIMISTIC, config->get_value(WRITERS), config->get_cnt_elements(WRITERS));
    run_readers_test(SET_OPTIMISTIC, config->get_value(READERS), config->get_cnt_elements(READERS));
    run_common_test(SET_OPTIMISTIC, config->get_value(COMMON_READERS), config->get_value(COMMON_WRITERS), config->get_value(COMMON_N));
    return 0;
}
