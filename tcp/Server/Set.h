//
// Created by evgenii on 31.01.18.
//

#ifndef ECHO_SET_H
#define ECHO_SET_H

#include <string.h>
#include <stdlib.h>

typedef struct set {
    unsigned long size;
    int* array;
} set_t;

int CreateSet(unsigned long Size, set_t** Set);
int SetInsert(set_t* Set, int Value);
int SetErase(set_t* Set, int Value);

#endif //ECHO_SET_H
