#include "socket_helpers.h"


int create_socket(int port, int backlog, char **err) {
    int sockfd, result;
    struct sockaddr_in6 addr;

    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        *err = "Could not create socket";
        return -1;
    }

    if(set_socket_options(sockfd, err) < 0)
        return -1;

    if(set_non_blocking(sockfd, err) < 0)
        return -1;

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sockfd);

        *err = "Could not bind socket";
        return -1;
    }

    if(listen(sockfd, backlog) < 0){
        close(sockfd);

        *err = "Could not listen on socket";
        return -1;
    }

    return sockfd;
}

int set_socket_options(int sockfd, char **err) {
    int on = 1, off = 0;

    #ifdef SO_REUSEPORT
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on)) < 0){
        close(sockfd);

        *err = "Could not set REUSEPORT flag";
        return -1;
    }
    #else
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0){
        close(sockfd);

        *err = "Could not set REUSEADDR flag";
        return -1;
    }
    #endif

    if(setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&off, sizeof(off)) < 0){
        close(sockfd);

        *err = "Could not set IPV6_V6ONLY flag";
        return -1;
    }

    return 0;
}


int set_non_blocking(int sockfd, char **err) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        flags = 0;

    if(fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)  < 0){
        close(sockfd);

        *err = "Could not set nonblocking";
        return -1;
    }

    return 0;
}


//int main(void) {
//    char *err;
//    int s;
//    if((s = create_socket(8080, 20, &err)) < 0){
//        printf("%s\n", err);
//    }
//
//    int n = accept(s, NULL, NULL);
//    char buffer[2048];
//
//    recv(n, buffer, sizeof(buffer), 0);
//    printf("%s\n", buffer);
//    close(s);
//}
