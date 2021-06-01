from coro cimport Coroutine

cdef class Loop:

    def create_coro(self, callback, *args, **kwargs):
        return Coroutine.__new__(Coroutine, callback, args, kwargs)

    def run(self, coroutines):
        return self._run(coroutines)

    cdef list _run(self, list coroutines):
        resps = []
        for i in range(len(coroutines)):
            coro = <Coroutine> coroutines[i]
            coro.coro_resume()
            resps.append(coro.response())

        return resps
