#include <stdio.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>

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
    pool_t *p = init_pool(coro_size(), 1);

    printf("%d\n", p->unit_size);
    coro_switcher_t switcher;
    printf("coro_new coro1 and coro2\n");
    coro_t *coro1 = coro_new(&switcher, &coro_func_foo, NULL, p);
    coro_t *coro2 = coro_new(&switcher, &coro_func_bar, NULL, p);
    coro_t *coro3 = coro_new(&switcher, &coro_func_bar, NULL, p);
    coro_t *coro4 = coro_new(&switcher, &coro_func_bar, NULL, p);
    coro_t *coro5 = coro_new(&switcher, &coro_func_bar, NULL, p);
    coro_t *coro6 = coro_new(&switcher, &coro_func_bar, NULL, p);
    printf("\nEnded new\n");

    coro_resume(coro1);
    coro_resume(coro2);
    coro_resume(coro3);
    coro_resume(coro4);
    coro_resume(coro5);
    coro_resume(coro6);
    printf("\nEnded first run\n");

    coro_resume(coro1);
    coro_resume(coro2);
    coro_resume(coro3);
    coro_resume(coro4);
    coro_resume(coro5);
    coro_resume(coro6);

    printf("coro_free coro1 and coro2\n");
    coro_free(coro1, p);
    coro_free(coro2, p);
    coro_free(coro3, p);
    coro_free(coro4, p);
    coro_free(coro5, p);
    coro_free(coro6, p);

    free_pool(p);

    return 0;
}
