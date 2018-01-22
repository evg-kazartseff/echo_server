//
// Created by evgenii on 22.01.18.
//

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        perror("Use TCP_Client \"IP address\" \"port\"");
        return 1;
    }
    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
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
        close(sock);
        return 1;
    };
    addr.sin_addr = ip_addr;
    long port = strtol(argv[2], NULL, 10);
    if (port < 0) {
        perror("Error: Port can't be negative");
        close(sock);
        return 1;
    } else if (port > USHRT_MAX) {
        perror("Error: Port can't be more than 65535");
        close(sock);
        return 1;
    }
    addr.sin_port = htons((uint16_t) port);
    char* buff = malloc(1024 * sizeof(char));
    strcpy(buff, "Hello world");
    sendto(sock, buff, 11, 0, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    printf("Send: ");
    printf("%s", buff);
    printf("\n");
    ssize_t bytes_read;
    socklen_t socklen;
    if ((bytes_read = recvfrom(sock, buff, 1024, 0, (struct sockaddr*) &addr, &socklen)) <= 0) {
        close(sock);
    }
    printf("Recv: ");
    printf("%s", buff);
    printf("\n");
}