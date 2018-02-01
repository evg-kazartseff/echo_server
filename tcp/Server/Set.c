//
// Created by evgenii on 31.01.18.
//

#include "Set.h"

int CreateSet(unsigned long Size, set_t** Set) {
    set_t* set = malloc(sizeof(set_t));
    if (!set)
        return 1;
    set->array = malloc(Size * sizeof(int));
    set->size = Size;
    memset(set->array, -1, set->size);
    if (!set->array)
        return 1;
    *Set = set;
    return 0;
}

int SetInsert(set_t* Set, int Value) {
    if (!Set)
        return 1;
    for (int i = 0; i < Set->size; i++) {
        if (Set->array[i] == -1) {
            Set->array[i] = Value;
            break;
        }
    }
    return 0;
}

int SetErase(set_t *Set, int Value) {
    if (!Set)
        return 1;
    for (int i = 0; i < Set->size; i++) {
        if (Set->array[i] == Value) {
            Set->array[i] = -1;
            break;
        }
    }
    return 0;
}
