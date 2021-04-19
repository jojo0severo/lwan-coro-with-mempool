#pragma once

#if defined (__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#if defined(__x86_64__)
#include <stdint.h>
typedef uintptr_t coro_context[10];
#elif defined(__i386__)
#include <stdint.h>
typedef uintptr_t coro_context[7];
#else
#include <ucontext.h>
typedef ucontext_t coro_context;
#endif

#include "mem-pool.h"

#define DEFAULT_BUFFER_SIZE 4096
#define ALWAYS_INLINE inline __attribute__((always_inline))

struct coro;
typedef struct coro coro_t;

typedef int    (*coro_function_t)	(struct coro *coro, void *data);

typedef struct coro_switcher {
    coro_context caller;
    coro_context callee;
}coro_switcher_t;

struct coro *coro_new(struct coro_switcher *switcher, coro_function_t function, void *data, pool_t *pool);
void	coro_free(struct coro *coro, pool_t *pool);

void    coro_reset(struct coro *coro, coro_function_t func, void *data);
void    coro_update(struct coro *coro, struct coro_switcher *new_switcher);

int	coro_resume(struct coro *coro);
int	coro_resume_value(struct coro *coro, int value);
int	coro_yield(struct coro *coro, int value);

void    coro_defer(struct coro *coro, void (*func)(void *data), void *data);
void    coro_defer2(struct coro *coro, void (*func)(void *data1, void *data2),
            void *data1, void *data2);

void    coro_deferred_run(struct coro *coro, size_t generation);
size_t  coro_deferred_get_generation(const struct coro *coro);

void   *coro_malloc(struct coro *coro, size_t sz)
            __attribute__((malloc));
void   *coro_malloc_full(struct coro *coro, size_t size, void (*destroy_func)())
            __attribute__((malloc));
char   *coro_strdup(struct coro *coro, const char *str);
char   *coro_strndup(struct coro *coro, const char *str, size_t len);
char   *coro_printf(struct coro *coro, const char *fmt, ...);
size_t coro_size();

#define CORO_DEFER(fn)		((void (*)(void *))(fn))
#define CORO_DEFER2(fn)		((void (*)(void *, void *))(fn))

#if defined (__cplusplus)
}
#endif