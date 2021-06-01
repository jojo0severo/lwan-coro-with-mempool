#cython: language_level=3
from libc.stdio cimport printf

cdef class Loop:
    cdef list _run(self, list coroutines)
