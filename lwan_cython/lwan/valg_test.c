#include <stdio.h>
#include <limits.h>
#include <pthread.h>
#include "lwan-coro.h"
#include "mem-pool.h"


int coro_func_foo(coro_t *coro, void *data) {
    printf("called coro_func_foo\n");
    printf("yield in coro_func_foo\n");
    coro_yield(coro, 0);
    printf("continue in coro_func_foo\n");
    return 0;
}

int coro_func_bar(coro_t *coro, void *data) {
    printf("called coro_func_bar\n");
    printf("yield in coro_func_bar\n");
    coro_yield(coro, 0);
    printf("continue in coro_func_bar\n");
    return 0;
}


int main() {
    pool_t *p = init_pool(sizeof(coro_t *) + ((3 * (PTHREAD_STACK_MIN)) / 2), 10);
//    free_pool(p);
//    return 0;
    coro_switcher_t switcher;
    printf("coro_new coro1 and coro2\n");
    coro_t *coro1 = coro_new(&switcher, &coro_func_foo, NULL, p);
    coro_t *coro2 = coro_new(&switcher, &coro_func_bar, NULL, p);

    coro_resume(coro1);
    coro_resume(coro2);

    coro_resume(coro1);
    coro_resume(coro2);

    printf("coro_free coro1 and coro2\n");
    coro_free(coro1, p);
    coro_free(coro2, p);

    free_pool(p);

    return 0;
}
/*

coro_new coro1 and coro2
called coro_func_foo
yield in coro_func_foo
called coro_func_bar
yield in coro_func_bar

*/