//
// Created by evgenii on 22.01.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include "Server.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        perror("Use TCP_Server \"IP address\" \"port\"");
        return 1;
    }

    long port = strtol(argv[2], NULL, 10);
    if (port < 0) {
        perror("Error: Port can't be negative");
        return 1;
    } else if (port > USHRT_MAX) {
        perror("Error: Port can't be more than 65535");
        return 1;
    }

    ServerRun(argv[1], (uint16_t) port);
    /*int sd_client;
    char* buff = malloc(1024 * sizeof(char));
    while (run) {
        if((sd_client = accept(sd_listener, NULL, NULL)) < 0) {
            perror("Error: Can't accept connection");
        }
        while (run) {
            bytes_read = recv(sd_client, buff, 1024, 0);
            if (bytes_read <= 0)
                break;
            buff[bytes_read] = '\000';
            if (bytes_read <= 1019) {
                strcat(buff, " - ok");
                bytes_read += 5;
            }
            send(sd_client, buff, (size_t) bytes_read, 0);
        }
        close(sd_client);
    }
    if (sd_listener > 0)
        close(sd_listener);
    if (buff)
        free(buff);
    */
    return 0;
}