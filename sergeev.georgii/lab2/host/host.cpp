#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "wolf.h"
#include <iostream>


int main () {
    wolf* host = wolf::get_instance();
    host->start();
    wolf::release_instance();
    return EXIT_SUCCESS;
}
