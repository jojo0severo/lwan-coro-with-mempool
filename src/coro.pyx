cdef class Coroutine:
    def __cinit__(self, Switcher switcher, Bridge bridge):
        self.switcher = switcher
        self.bridge = bridge
        self.coroutine = coro_new(switcher.switcher, callback, <void*> bridge)
        if self.coroutine is NULL:
            raise MemoryError()

    cpdef void coro_reset(self, Bridge data):
        coro_reset(self.coroutine, callback, <void *> data)

    cpdef void coro_update(self, Switcher new_switcher):
        coro_update(self.coroutine, new_switcher.switcher)

    cpdef int coro_resume(self):
        return coro_resume(self.coroutine)

    cpdef int coro_resume_value(self, int value):
        return coro_resume_value(self.coroutine, value)

    cpdef int coro_yield(self, int value):
        return coro_yield(self.coroutine, value)

    cpdef int coro_size(self):
        return coro_size()

    cpdef object response(self):
        return <object> self.bridge.response

    def __dealloc__(self):
        if self.coroutine is not NULL:
            coro_free(self.coroutine)

cdef class Switcher:
    def __cinit__(self):
        self.switcher = <coro_switcher_t *> malloc(sizeof(coro_switcher_t))

    def __dealloc__(self):
        free(self.switcher)

cdef class Bridge:
    def __cinit__(self, callback, arguments):
        Py_INCREF(callback)
        Py_INCREF(arguments)

        self.callback = <void*> callback
        self.arguments = <void*> arguments

    def __dealloc__(self):
        pass

cdef int callback(coro_t *coro, void *data) nogil:
    with gil:
        python_data = <Bridge> (<object> data)
        callback = <object> python_data.callback
        arguments = <object> python_data.arguments
        response = <object> callback(arguments)

        Py_INCREF(response)
        python_data.response = <void*> response

    return 0
