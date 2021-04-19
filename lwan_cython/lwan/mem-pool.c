#define _GNU_SOURCE
#include <stdio.h>

#include "mem-pool.h"

void
more_mem(pool_t *pool, size_t new_size)
{
    printf("MAIS MEMORIAAA\n");
    node_t *aux;

    size_t block_size = sizeof(node_t) + pool->unit_size;
    void *mem = malloc(sizeof(node_t) + new_size * block_size);

    aux = mem;
    aux->next = pool->addr_head;
    pool->addr_head = aux;

    mem += sizeof(node_t);

    for(int i = 0; i < new_size; i++)
    {
        aux = mem;
        aux->next = pool->head;
        pool->head = aux;

        mem += block_size;
    }
}

pool_t *
init_pool(size_t unit_size, size_t units_amount)
{
    pool_t *pool = malloc(sizeof(pool_t));

    pool->head = NULL;
    pool->addr_head = NULL;
    pool->unit_size = unit_size;
    if(units_amount / 2)
        pool->increment = (size_t) (units_amount / 2);
    else
        pool->increment = 1;

    more_mem(pool, units_amount);

    return pool;
}

void *
alloc_mem(pool_t *pool)
{
    void *alloc_node;

    if(pool->head == NULL)
    {
        more_mem(pool, pool->increment);
    }

    alloc_node = (void *) (pool->head);
    pool->head = pool->head->next;

    return alloc_node + sizeof(node_t);
}

void
free_mem(pool_t *pool, void *alloc_node)
{
    size_t len = pool->unit_size;
    unsigned char *empty = alloc_node;
    while (len-- > 0)
        *empty++ = 0;

    node_t *aux_node = (node_t *)(alloc_node - sizeof(node_t));
    aux_node->next = pool->head;
    pool->head = aux_node;
}

void
free_pool(pool_t *pool)
{
   node_t *tmp_addr_head;

   while (pool->addr_head != NULL)
   {
       tmp_addr_head = pool->addr_head;
       pool->addr_head = pool->addr_head->next;
       free(tmp_addr_head);
   }
   free(pool);
}

//int
//main()
//{
//    unsigned int i;
//    unsigned int j;
//    clock_t begin, end;
//
//    pool_t *p;
//
//    for(i = 500; i <= 5000; i+=500)
//    {
//        p = init_pool(100, 3000);
//        printf("\n---------------- Size %u ---------------\n", i);
//        begin = clock();
//        for(j = 0; j < i; j++)
//        {
//            malloc(100);
//        }
//        end = clock();
//        printf("Malloc Time: %f\n", (double)(end - begin) / CLOCKS_PER_SEC);
//
//        begin = clock();
//        for(j = 0; j < i; j++)
//        {
//            alloc_mem(p);
//        }
//        end = clock();
//        printf("Pool Time: %f\n", (double)(end - begin) / CLOCKS_PER_SEC);
//        free_pool(p);
//    }
//}