from coro cimport Coroutine, Switcher, Bridge

cdef class Loop:

    cpdef create_coro(self, callback, arguments):
        switcher = Switcher.__new__(Switcher)
        bridge = Bridge.__new__(Bridge, callback, arguments)

        return Coroutine.__new__(Coroutine, switcher, bridge)

    cpdef run(self, coroutines):
        resps = []
        for coro in coroutines:
            coro.coro_resume()
            resps.append(coro.response())

        return resps
