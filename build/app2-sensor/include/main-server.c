#include "server.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    Server server = server_new();
    server_run(&server);
    server_free(server);
}
