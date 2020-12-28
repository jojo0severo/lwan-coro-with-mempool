#define _GNU_SOURCE

#include "mem-pool.h"

void
more_mem(pool_t *pool, size_t new_size)
{
    node_t *tmp_head, *tmp_tail, *curr_head;
    addr_node_t *start;
    size_t i, block_size;
    void *mem;

    block_size = sizeof(node_t) + pool->unit_size;
    mem = malloc(sizeof(addr_node_t) + new_size * block_size);
    start = mem;

    mem += sizeof(addr_node_t);

    curr_head = mem;
    curr_head->next = NULL;
    mem += block_size;

    tmp_tail = curr_head;

    curr_head = mem;
    curr_head->next = tmp_tail;
    mem += block_size;

    tmp_head = curr_head;

    for(i = 2; i < new_size; i++)
    {
        curr_head = mem;
        curr_head->next = tmp_head;
        tmp_head = curr_head;

        mem += block_size;
    }

    pthread_mutex_lock(&pool->head_lock);

    if(pool->head != NULL)
        tmp_tail->next = pool->head;
    pool->head = tmp_head;

    start->next = pool->addr_head;
    pool->addr_head = start;

    pthread_mutex_unlock(&pool->head_lock);
    pthread_mutex_unlock(&pool->more_mem_lock);
}

pool_t *
init_pool(size_t unit_size, size_t units_amount)
{
    pool_t *pool = malloc(sizeof(pool_t));
    pool->addr_head = malloc(sizeof(addr_node_t) + sizeof(node_t));
    pool->addr_head->next = NULL;
    pool->tail = NULL;
    pool->head = (node_t *) (pool->addr_head + sizeof(addr_node_t));
    pool->head->next = pool->tail;

    pool->unit_size = unit_size;
    pool->increment = (int) (units_amount / get_nprocs_conf());

    pthread_mutex_init(&pool->more_mem_lock, NULL);
    pthread_mutex_init(&pool->head_lock, NULL);
    pthread_mutex_init(&pool->tail_lock, NULL);

    more_mem(pool, units_amount);

    pool->tail = (node_t *) (((void *) pool->addr_head) + sizeof(addr_node_t));

    return pool;
}

void *
alloc_mem(pool_t *pool)
{
    void *alloc_node;

    pthread_mutex_lock(&pool->head_lock);
    if(pool->head == pool->tail)
    {
        addr_node_t *new_addr = malloc(sizeof(addr_node_t) + sizeof(node_t));
        new_addr->next = pool->addr_head;
        pool->addr_head = new_addr;

        pthread_mutex_unlock(&pool->head_lock);

        alloc_node = (void *) (new_addr + sizeof(addr_node_t));

        if(pthread_mutex_trylock(&pool->more_mem_lock) == 0)
        {
            more_mem(pool, pool->increment);
        }
    }
    else
    {
        alloc_node = (void *) (pool->head);
        pool->head = pool->head->next;

        pthread_mutex_unlock(&pool->head_lock);
    }

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

    pthread_mutex_lock(&pool->tail_lock);

    pool->tail->next = aux_node;
    pool->tail = aux_node;

    pthread_mutex_unlock(&pool->tail_lock);
}

void
free_pool(pool_t *pool)
{
   addr_node_t *tmp_addr_head;

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