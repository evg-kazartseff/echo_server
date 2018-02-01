//
// Created by evgenii on 28.01.18.
//

#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include <pthread.h>
#include "Queue.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "Set.h"

#define N_HANDLERS 5

int ServerRun(char* IP_address, uint16_t port);

#endif //ECHO_SERVER_H