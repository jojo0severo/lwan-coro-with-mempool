#cython: language_level=3

from libc.stdio cimport printf
from libc.stdlib cimport malloc, free
from cpython.ref cimport Py_INCREF, Py_DECREF, PyObject


cdef extern from "lwan-coro.h" nogil:
    ctypedef struct coro_switcher_t:
        pass
    ctypedef struct coro_t:
        coro_switcher_t *switcher

    ctypedef int (*coro_function_t)(coro_t *, void *)

    coro_t *coro_new(coro_switcher_t *, coro_function_t, void *)
    void coro_reset(coro_t *, coro_function_t, void *)
    void coro_update(coro_t *, coro_switcher_t *)
    int coro_resume(coro_t *)
    int coro_resume_value(coro_t *, int)
    int coro_yield(coro_t *, int)
    size_t coro_size()
    void coro_free(coro_t *)

cdef enum coro_yield_value:
    ABORT = -1,
    MAY_RESUME = 0,
    FINISHED = 1

cdef class Switcher:
    cdef coro_switcher_t *switcher;

cdef class Bridge:
    cdef:
        void *callback;
        void *arguments;
        void *response;

cdef class Coroutine:
    cdef:
        coro_t *coroutine;
        Switcher switcher;
        Bridge bridge;

    cpdef void coro_reset(self, Bridge bridge)
    cpdef void coro_update(self, Switcher new_switcher)
    cpdef int coro_resume(self)
    cpdef int coro_resume_value(self, int value)
    cpdef int coro_yield(self, int value)
    cpdef int coro_size(self)
    cpdef object response(self)
