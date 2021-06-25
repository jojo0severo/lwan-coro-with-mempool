#ifndef DICT_H
#define DICT_H

#define SEED 61987

##include "../external/xxhash.h"


typedef struct element{
    char *key;
    char *key_type;

    char *value;
    char *value_type;

    bool parsed;

} Element;


##endif