cdef class Coroutine:
    def __cinit__(self, object callback, object args, object kwargs):
        cdef:
            PyObject *c_callback;
            PyObject *c_args;
            PyObject *c_kwargs;

        c_callback = <PyObject *> callback
        c_callback.ob_refcnt += 1

        c_args = <PyObject *> args
        c_args.ob_refcnt += 1

        c_kwargs = <PyObject *> kwargs
        c_kwargs.ob_refcnt += 1

        self.bridge = <bridge_t *> malloc(sizeof(bridge_t))
        self.bridge.callback = c_callback
        self.bridge.arguments = c_args
        self.bridge.key_arguments = c_kwargs

        self.switcher = <coro_switcher_t *> malloc(sizeof(coro_switcher_t))
        self.coroutine = coro_new(self.switcher, callback_wrap, <void *> self.bridge)

    cdef void _coro_reset(self, object callback, object args, object kwargs):
        cdef:
            PyObject *c_callback;
            PyObject *c_args;
            PyObject *c_kwargs;

        c_callback = <PyObject *> callback
        c_callback.ob_refcnt += 1

        c_args = <PyObject *> args
        c_args.ob_refcnt += 1

        c_kwargs = <PyObject *> kwargs
        c_kwargs.ob_refcnt += 1

        free(self.bridge)

        self.bridge = <bridge_t *> malloc(sizeof(bridge_t))
        self.bridge.callback = c_callback
        self.bridge.arguments = c_args
        self.bridge.key_arguments = c_kwargs

        coro_reset(self.coroutine, callback_wrap, <void *> self.bridge)

    cdef void _coro_update(self, coro_switcher_t *switcher):
        coro_update(self.coroutine, switcher)

    def coro_resume(self):
        return self._coro_resume()

    cdef int _coro_resume(self):
        return coro_resume(self.coroutine)

    cdef int _coro_resume_value(self, int value):
        return coro_resume_value(self.coroutine, value)

    def coro_yield(self):
        self._coro_yield(1)

    cdef int _coro_yield(self, int value):
        return coro_yield(self.coroutine, value)

    cdef int _coro_size(self):
        return coro_size()

    cdef object _response(self):
        cdef PyObject *resp = self.bridge.response
        if resp == NULL:
            return None

        return <object> resp

    def __dealloc__(self):
        if self.bridge.callback != NULL:
            self.bridge.callback.ob_refcnt -= 1
        if self.bridge.arguments != NULL:
            self.bridge.arguments.ob_refcnt -= 1
        if self.bridge.key_arguments != NULL:
            self.bridge.key_arguments.ob_refcnt -= 1
        if self.bridge.response != NULL:
            self.bridge.response.ob_refcnt -= 1

        free(self.bridge)
        free(self.switcher)

        coro_free(self.coroutine)

cdef int callback_wrap(coro_t *coro, void *data) nogil:
    cdef:
        bridge_t *bridge;
        PyObject *c_callback;
        PyObject *c_arguments;
        PyObject *c_key_arguments;
        PyObject *c_response;

    bridge = <bridge_t *> data
    c_callback = bridge.callback
    c_arguments = bridge.arguments
    c_key_arguments = bridge.key_arguments
    c_response = NULL

    with gil:
        args = <object> c_arguments
        kwargs = <object> c_key_arguments
        callback = <object> c_callback

        tmp = callback(*args, **kwargs)

        response = <PyObject*> tmp

    response.ob_refcnt += 1
    bridge.response = response

    return 0
