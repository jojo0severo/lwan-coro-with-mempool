
#pragma once

#if defined (__cplusplus)
extern "C" {
#endif

#include <sys/sysinfo.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct node {
    struct node *next;
} node_t;

typedef struct addr_node {
    struct addr_node *next;
} addr_node_t;

typedef struct pool {
    node_t *head;
    node_t *tail;
    addr_node_t *addr_head;

    int increment;
    size_t unit_size;

    pthread_mutex_t more_mem_lock;
    pthread_mutex_t head_lock;
    pthread_mutex_t tail_lock;

} pool_t;


pool_t *init_pool(size_t unit_size, size_t units_amount);
void more_mem(pool_t *pool, size_t new_size);
void *alloc_mem(pool_t *pool);

void free_mem(pool_t *pool, void *alloc_node);
void free_pool(pool_t *pool);

#if defined (__cplusplus)
}
#endif