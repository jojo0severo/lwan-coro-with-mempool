#cython: language_level=3


cdef class Loop:
    cpdef create_coro(self, callback, arguments)
    cpdef run(self, coroutines)