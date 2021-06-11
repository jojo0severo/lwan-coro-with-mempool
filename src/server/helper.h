#ifndef HELPER_H
#define HELPER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/mman.h>

int create_socket(int port, int backlog);
void set_socket_options(int sockfd);
void set_non_blocking(int sockfd);

void *create_shared_memory(size_t size);
char *get_response();

#endif