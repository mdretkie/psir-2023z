#include "server.h"
#include "common.h"
#include "app1.h"
#include "app2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid arguments\n");
        exit(EXIT_FAILURE);
    }

    if (!strcmp(argv[1], "server")) {
        printf("%s Server starting\n", formatted_timestamp());
        Server server = server_new();
        printf("%s Initialisation done. Listening for messages\n", formatted_timestamp());
        server_run(&server);
        server_free(server);
        printf("%s Server finished\n", formatted_timestamp());
    } else if (!strcmp(argv[1], "app1")) {
        app1_main();
    } else if (!strcmp(argv[1], "app2")) {
        app2_main();
    } else {
        printf("Invalid arguments\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
