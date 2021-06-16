

header_t terminator = {.name = "\0", .value = "\0"};

int get_method(char **request, method_t *method, size_t request_length) {
    register int i, previous = 0;

    // Get the request type
    for(i = 0; i < request_length; i++){
        if((*request)[i] == ' '){
            method->type = malloc(i * sizeof(char));
            memcpy(method->type, &(*request)[previous], i);

            i++;
            previous = i;

            goto url;
        }
    }

    // Error occurred when retrieving the request type
    return -1;

    // Get the request URL
    url:
    for(; i < request_length; i++){
        if((*request)[i] == ' '){
            method->url = malloc((i - previous) * sizeof(char));
            memcpy(method->url, &(*request)[previous], i - previous);

            i++;
            previous = i;

            goto version;
        }
    }

    // Error occurred when retrieving the request URL
    return -2;

    // Get the request http version
    version:
    for(; i < request_length; i++){
        if((*request)[i] == '\r' && (*request)[i+1] == '\n'){
            method->version = malloc((i - previous) * sizeof(char));
            memcpy(method->version, &(*request)[previous], i - previous);

            previous = i + 2;

            goto end;
        }
    }

    // Error occurred when retrieving the request version
    return -3;

    end:
    *request = (*request) + previous;
    return 0;
}


int get_headers(char **request, header_t **headers, size_t request_length) {
    register size_t length = 30;
    *headers = malloc(30 * sizeof(header_t));

    header_t aux;
    register int i, counter = 0, previous = 0;
    for (i = 0; i < request_length - 1; i++, counter++){
        if(i >= length - 1){
            length = i + 10;
            *headers = realloc(*headers, length * sizeof(header_t));
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

        // Error occurred when retrieving the header name
        return -1;

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

        // Error occurred when retrieving the header value
        return -2;

        add:
        (*headers)[counter] = aux;

        if((*request)[i] == '\r' && (*request)[i+1] == '\n'){
            previous = i + 2;
            goto end;
        }
    }

    // Error occurred when adding current header
    return -3;

    end:
    *request = (*request) + previous;
    (*headers)[++counter] = terminator;

    return 0;
}

//int main(void)
//{
//    char *test = "GET / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\nsec-ch-ua: \" Not;A Brand\";v=\"99\", \"Google Chrome\";v=\"91\", \"Chromium\";v=\"91\"\r\nsec-ch-ua-mobile: ?0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.77 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: pt-BR,pt;q=0.9,en;q=0.8\r\nCookie: ajs_user_id=%222f9bdd6a-364e-54fe-8971-27c658745e57%22; ajs_anonymous_id=%2210deac30-2143-40f1-adc6-9d98e27c53e4%22; _hp2_id.1823968819=%7B%22userId%22%3A%228018640704986820%22%2C%22pageviewId%22%3A%225512550737323674%22%2C%22sessionId%22%3A%222579518704287441%22%2C%22identity%22%3A%222f9bdd6a-364e-54fe-8971-27c658745e57%22%2C%22trackerVersion%22%3A%224.0%22%2C%22identityField%22%3Anull%2C%22isIdentified%22%3A1%2C%22oldIdentity%22%3Anull%7D\r\n\r\nblabla";
//    size_t request_length = strlen(test);
//
//    method_t method;
//    get_method(&test, &method, request_length);
//
//    header_t *headers;
//    get_headers(&test, &headers, request_length);
//
//    return 0;
//}