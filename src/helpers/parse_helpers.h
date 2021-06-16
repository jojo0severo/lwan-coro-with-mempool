#ifndef PARSE_H
#define PARSE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct method_def {
    char *type;
    char *url;
    char *version;
} method_t;

typedef struct header_def {
    char *name;
    char *value;
} header_t;

int get_method(char **request, method_t *method, size_t request_length);
int get_headers(char **request, header_t **headers, size_t request_length);

#endif