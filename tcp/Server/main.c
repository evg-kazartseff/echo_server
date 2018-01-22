//
// Created by evgenii on 22.01.18.
//

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>


int run = 1;

void interapt(int signo) {
    if (signo == SIGINT) {
        run = 0;
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        perror("Use TCP_Server \"IP address\" \"port\"");
        return 1;
    }
    signal(SIGINT, interapt);
    int sd_listener;
    if ((sd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Error: Con't create socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    struct in_addr ip_addr;
    memset(&ip_addr, 0, sizeof(struct in_addr));
    if(inet_aton(argv[1], &ip_addr) == 0) {
        perror("Error: Can't fill ip address");
        close(sd_listener);
        return 1;
    };
    addr.sin_addr = ip_addr;
    long port = strtol(argv[2], NULL, 10);
    if (port < 0) {
        perror("Error: Port can't be negative");
        close(sd_listener);
        return 1;
    } else if (port > USHRT_MAX) {
        perror("Error: Port can't be more than 65535");
        close(sd_listener);
        return 1;
    }
    addr.sin_port = htons((uint16_t) port);
    if(bind(sd_listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error: Con't bind socket");
        return 1;
    }
    if(listen(sd_listener, 5) == -1) {
        perror("Error: Listen error");
        return 1;
    }
    ssize_t bytes_read;
    int sd_client;
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
    return 0;
}