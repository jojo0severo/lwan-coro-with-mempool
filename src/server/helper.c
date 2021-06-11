#include "helper.h"


void *create_shared_memory(size_t size) {
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  return mmap(NULL, size, protection, visibility, -1, 0);
}


int create_socket(int port, int backlog){
    int sockfd, result;
    struct sockaddr_in6 addr;

    if ((sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(-1);
    }

    set_socket_options(sockfd);
    set_non_blocking(sockfd);

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    addr.sin6_addr = in6addr_any;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind");
        close(sockfd);
        exit(-1);
    }

    if(listen(sockfd, backlog) < 0){
        perror("listen");
        close(sockfd);
        exit(-1);
    }

    return sockfd;
}

void set_socket_options(int sockfd){
    int on = 1, off = 0;

    #ifdef SO_REUSEPORT
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on)) < 0){
        perror("reuseport");
        close(sockfd);
        exit(-1);
    }
    #else
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0){
        perror("reuseaddr");
        close(sockfd);
        exit(-1);
    }
    #endif

    if(setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&off, sizeof(off)) < 0){
        perror("setsockopt");
        close(sockfd);
        exit(-1);
    }
}


void set_non_blocking(int sockfd){
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1){
        flags = 0;
    }

    if(fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)  < 0){
        perror("nonblocking");
        close(sockfd);
        exit(-1);
    }
}


char *get_response() {
    char *response = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 96\r\n\r\n"
                    "<!DOCTYPE html>\r\n<html><head><title>Testing</title></head>\r\n<body><p>Testing</p></body><html>\r\n\0";

    return response;
}