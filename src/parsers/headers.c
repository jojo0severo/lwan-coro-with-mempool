#include "parse_helpers.h"

header_t terminator = {.name = "\0", .value = "\0"};

method_t get_method(char **request, size_t request_length, char **error) {
    register int i, previous = 0;

    method_t method;

    // Get the request type
    for(i = 0; i < request_length; i++){
        if((*request)[i] == ' '){
            method.type = malloc(i * sizeof(char));
            memcpy(method.type, &(*request)[previous], i);

            i++;
            previous = i;

            goto url;
        }
    }

    *error = "Error occurred when retrieving the request type";
    return method;

    // Get the request URL
    url:
    for(; i < request_length; i++){
        if((*request)[i] == ' '){
            method.url = malloc((i - previous) * sizeof(char));
            memcpy(method.url, &(*request)[previous], i - previous);

            i++;
            previous = i;

            goto version;
        }
    }

    *error = "Error occurred when retrieving the request URL";
    return method;

    // Get the request http version
    version:
    for(; i < request_length; i++){
        if((*request)[i] == '\r' && (*request)[i+1] == '\n'){
            method.version = malloc((i - previous) * sizeof(char));
            memcpy(method.version, &(*request)[previous], i - previous);

            previous = i + 2;

            goto end;
        }
    }

    *error = "Error occurred when retrieving the request version";
    return method;

    end:
    *request = (*request) + previous;

    return method;
}

header_t *get_headers(char **request, size_t request_length, char **error) {
    register size_t length = 30;
    headers = malloc(30 * sizeof(header_t));

    header_t aux;
    register int i, counter = 0, previous = 0;
    for (i = 0; i < request_length - 1; i++, counter++){
        if(i >= length - 1){
            length = i + 10;
            headers = realloc(headers, length * sizeof(header_t));
        }

        name:
        for(; i < request_length; i++){
            if((*request)[i] == ':'){
                aux.name = malloc((i - previous) * sizeof(char));
                memcpy(aux.name, &(*request)[previous], i - previous);

                i += 2;
                previous = i;

                goto value;
            }
        }

        *error = "Error occurred when retrieving the header name";
        return headers;

        value:
        for(; i < request_length - 1; i++){
            if((*request)[i] == '\r' && (*request)[i+1] == '\n'){
                aux.value = malloc((i - previous) * sizeof(char));
                memcpy(aux.value, &(*request)[previous], i - previous);

                i += 2;
                previous = i;

                goto add;
            }
        }

        *error = "Error occurred when retrieving the header value";
        return headers;

        add:
        headers[counter] = aux;

        if((*request)[i] == '\r' && (*request)[i+1] == '\n'){
            previous = i + 2;
            goto end;
        }
    }

    *error = "Error occurred when adding current header";
    return headers;

    end:
    *request = (*request) + previous;
    headers[++counter] = terminator;

    return headers;
}