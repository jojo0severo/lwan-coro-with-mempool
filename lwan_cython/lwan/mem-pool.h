
#pragma once

#if defined (__cplusplus)
extern "C" {
#endif

#include <stdlib.h>

typedef struct node {
    struct node *next;
} node_t;

typedef struct pool {
    node_t *head;
    node_t *addr_head;

    size_t increment;
    size_t unit_size;

} pool_t;


pool_t *init_pool(size_t unit_size, size_t units_amount);
void more_mem(pool_t *pool, size_t new_size);
void *alloc_mem(pool_t *pool);

void free_mem(pool_t *pool, void *alloc_node);
void free_pool(pool_t *pool);

#if defined (__cplusplus)
}
#endif