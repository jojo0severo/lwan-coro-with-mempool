#cython: language_level=3

from libc.stdlib cimport malloc, free
from cpython.ref cimport Py_INCREF, Py_DECREF, PyObject
from cpython.object cimport PyObject_Call, PyObject_CallObject
from cpython.method cimport PyMethod_GET_FUNCTION


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

cdef struct bridge_t:
    PyObject *callback;
    PyObject *arguments;
    PyObject *key_arguments;
    PyObject *response;

cdef class Coroutine:
    cdef:
        coro_t *coroutine;
        coro_switcher_t *switcher;
        bridge_t *bridge;

    cdef void coro_reset(self, object callback, object args, object kwargs)
    cdef void coro_update(self, coro_switcher_t *switcher)
    cdef int coro_resume(self)
    cdef int coro_resume_value(self, int value)
    cdef int coro_yield(self, int value)
    cdef int coro_size(self)
    cdef object response(self)
