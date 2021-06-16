#ifndef SOCKET_H
#define SOCKET_H

#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int create_socket(int port, int backlog, char **err);
int set_socket_options(int sockfd, char **err);
int set_non_blocking(int sockfd, char **err);

#endif