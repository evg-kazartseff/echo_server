//
// Created by evgenii on 28.01.18.
//

#include "Server.h"

enum {
    Start,
    Listen,
    SendTask,
    Error
};

enum {
    Success,
    Socket_err,
    InetAton_err,
    Bind_err,
    Listen_err,
    Listener_err,
    Handler_create_err,
    Queue_create_err,
    SendTask_err
};

enum {
    Test,
    Init,
    Err,
    Select,
    SendRecv
};

typedef struct Server {
    int sd_listener;
    struct sockaddr_in addr;

    pthread_t handler[N_HANDLERS];

    queue_t* sd_client_queue[N_HANDLERS];
    int handler_selector;
} Server_t;

typedef struct FSM {
    Server_t* Server;
    int State;
    int arg;
} FSM_t;

int run = 1;

void interapt(int signo) {
    if (signo == SIGINT) {
        run = 0;
    }
}

void* HandlerRoutine(void* arg) {
    int state = Init;
    queue_t* Queue = (queue_t*) arg;
    set_t* SetClient;
    CreateSet(2048, &SetClient);
    unsigned long QueueSize = 0;
    int flg;
    int state_arg = 0;
    struct timeval time;
    fd_set clients_sd;
    while (run) {
        switch (state) {
            case Init:
                flg = QueueGetSize(Queue, &QueueSize);
                if (flg) {
                    state_arg = flg;
                    state = Err;
                }
                else {
                    FD_ZERO(&clients_sd);
                    time.tv_sec = 1;
                    time.tv_usec = 0;
                    if (QueueSize > 0) {
                        for (int i = 0; i < QueueSize; ++i) {
                            int new_client;
                            flg = QueueErase(Queue, &new_client);
                            if (flg) {
                                state_arg = flg;
                                state = Err;
                                break;
                            }
                            else {
                                SetInsert(SetClient, new_client);
                            }
                        }
                    }
                    if (state != Err) {
                        for (int i = 0; i < SetClient->size; ++i) {
                            if (SetClient->array[i] != -1) {
                                FD_SET(SetClient->array[i], &clients_sd);
                            }
                        }
                        state = Select;
                    }
                }
                break;
            case Select:
                flg = select(1000, &clients_sd, NULL, NULL, &time);
                if(flg == -1)
                {
                    state_arg = flg;
                    state = Err;
                } else {
                    state = SendRecv;
                }
                break;
            case SendRecv:
                for (int i = 0; i < SetClient->size; ++i) {
                    if (FD_ISSET(SetClient->array[i], &clients_sd)) {
                        char buff[1024];
                        ssize_t bytes_read = recv(SetClient->array[i], buff, 1024, 0);
                        if (bytes_read <= 0) {
                            close(SetClient->array[i]);
                            SetErase(SetClient, SetClient->array[i]);
                            continue;
                        }
                        buff[bytes_read] = '\000';
                        if (bytes_read <= 1019) {
                            strcat(buff, " - ok");
                            bytes_read += 5;
                        }
                        send(SetClient->array[i], buff, (size_t) bytes_read, 0);
                    }
                }
                state = Init;
                break;
            case Err:
                if (state_arg == 1) {
                    perror("Error handler thread");
                }
                state = Init;
                break;
            default:
                break;
        }
    }
}

int CreateServer(char *IP_address, uint16_t port,  Server_t** Server) {
    signal(SIGINT, interapt);
    Server_t* server = malloc(sizeof(Server_t));
    if ((server->sd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        free(server);
        return Socket_err;
    }
    memset(&server->addr, 0, sizeof(struct sockaddr_in));
    server->addr.sin_family = AF_INET;
    struct in_addr ip_addr;
    memset(&ip_addr, 0, sizeof(struct in_addr));
    if(inet_aton(IP_address, &ip_addr) == 0) {
        close(server->sd_listener);
        free(server);
        return InetAton_err;
    };
    server->addr.sin_addr = ip_addr;
    server->addr.sin_port = htons(port);
    if(bind(server->sd_listener, (struct sockaddr *) &server->addr, sizeof(server->addr)) < 0) {
        close(server->sd_listener);
        free(server);
        return Bind_err;
    }
    if(listen(server->sd_listener, 10) == -1) {
        close(server->sd_listener);
        free(server);
        return Listen_err;
    }
    for (int i = 0; i < N_HANDLERS; ++i) {
        if (QueueCreate(&server->sd_client_queue[i])) {
            close(server->sd_listener);
            free(server);
            return Queue_create_err;
        }
    }
    for (int i = 0; i < N_HANDLERS; ++i) {
        if (pthread_create(&server->handler[i], NULL, HandlerRoutine, (void *) server->sd_client_queue[i])) {
            for (int j = 0; j < i; ++j) {
                pthread_cancel(server->handler[j]);
            }
            close(server->sd_listener);
            free(server);
            return Handler_create_err;
        }
    }
    *Server = server;
    return Success;
}

int ServerListen(Server_t* Server, int* client) {
    if((*client = accept(Server->sd_listener, NULL, NULL)) < 0) {
        return Listener_err;
    }
    return Success;
}

int ServerRun(char *IP_address, uint16_t port) {
    FSM_t FSM;
    FSM.State = Start;
    FSM.Server = NULL;
    while (run) {
        int flg;
        switch (FSM.State) {
            case Start:
                flg = CreateServer(IP_address, port, &FSM.Server);
                if (flg == Success) {
                    FSM.State = Listen;
                } else {
                    FSM.State = Error;
                    FSM.arg = flg;
                }
                break;
            case Listen:
                flg = ServerListen(FSM.Server, &FSM.arg);
                if (flg == Success) {
                    FSM.State = SendTask;
                }
                else {
                    FSM.State = Error;
                }
                break;
            case SendTask:
                flg = QueueInsert(FSM.Server->sd_client_queue[(FSM.Server->handler_selector++)%N_HANDLERS], FSM.arg);
                if (flg == 0) {
                    FSM.State = Listen;
                }
                else if (flg == 1) {
                    FSM.State = Error;
                    FSM.arg = SendTask_err;
                }
                break;
            case Error:
                if (FSM.arg == Socket_err) {
                    perror("Can't create socket");
                    FSM.State = Start;
                } else if (FSM.arg == InetAton_err) {
                    perror("Error: Can't fill ip address");
                    FSM.State = Start;
                } else if (FSM.arg == Bind_err) {
                    perror("Error: Con't bind socket");
                    FSM.State = Start;
                } else if (FSM.arg == Listen_err) {
                    perror("Error: Listen error");
                    FSM.State = Start;
                } else if (FSM.arg == Handler_create_err) {
                    perror("Error: Can't create handler");
                    FSM.State = Start;
                } else if (FSM.arg == Queue_create_err) {
                    perror("Error: Can't create queue");
                    FSM.State = Start;
                } else if (FSM.arg == Listener_err) {
                    perror("Error: Can't accept connection");
                    FSM.State = Listen;
                } else if (FSM.arg == SendTask_err) {
                    perror("Error: Can't send client descriptor to handler");
                    FSM.State = Listen;
                }
                break;
            default:
                FSM.State = Error;
                break;
        }
    }
}
