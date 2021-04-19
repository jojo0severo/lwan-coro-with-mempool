#cython: language_level=3

from libc.stdlib cimport exit, calloc, free, rand
from libc.stdio cimport printf, fprintf, stderr
from libc.string cimport memset
from posix.unistd cimport usleep, sysconf, sleep

cdef extern from "<unistd.h>" nogil:
  enum: _SC_NPROCESSORS_ONLN # Seems to no be included in "posix.unistd".

from libcpp.deque cimport deque
ctypedef deque[coro_t *] coro_deque_t

cdef extern from "lwan/lwan-coro.h" nogil:
  ctypedef struct coro_switcher_t:
    pass
  ctypedef struct coro_t:
    coro_switcher_t *switcher

  ctypedef int (*coro_function_t) (coro_t *, void *)

  void coro_reset(coro_t *, coro_function_t, void *)
  coro_t *coro_new(coro_switcher_t *, coro_function_t, void *)
  void coro_free(coro_t *)
  int coro_resume(coro_t *)
  int coro_resume_value(coro_t *, int)
  int coro_yield(coro_t *, int)

  void coro_update(coro_t *, coro_switcher_t *)

cdef enum coro_yield_value:
  ABORT = -1,
  MAY_RESUME = 0,
  FINISHED = 1

cdef extern from "<sys/types.h>" nogil:
  ctypedef long unsigned int pthread_t

  ctypedef union pthread_attr_t:
    pass
  ctypedef union pthread_mutex_t:
    pass
  ctypedef union pthread_mutexattr_t:
    pass
  ctypedef union pthread_barrier_t:
    pass
  ctypedef union pthread_barrierattr_t:
    pass

cdef extern from "<pthread.h>" nogil:
  int pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *)
  void pthread_exit(void *)
  int pthread_join(pthread_t, void **)  
  int pthread_attr_init(pthread_attr_t *)
  int pthread_attr_setdetachstate(pthread_attr_t *, int)
  int pthread_attr_destroy(pthread_attr_t *)

  int pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *)
  int pthread_mutex_destroy(pthread_mutex_t *)
  int pthread_mutex_lock(pthread_mutex_t *)
  int pthread_mutex_unlock(pthread_mutex_t *)
  int pthread_mutex_trylock(pthread_mutex_t *)

  int pthread_barrier_init(pthread_barrier_t *, const pthread_barrierattr_t *, unsigned int)
  int pthread_barrier_destroy(pthread_barrier_t *)
  int pthread_barrier_wait(pthread_barrier_t *)

  enum: PTHREAD_CREATE_JOINABLE

cdef extern from "<semaphore.h>" nogil:
  ctypedef int sem_t

  int sem_destroy(sem_t *)
  int sem_getvalue(sem_t *, int *)
  int sem_init(sem_t *, int, unsigned int)
  int sem_post(sem_t *)
  int sem_wait(sem_t *)

cdef struct scheduler_t:
  worker_t *workers
  unsigned short num_workers
  pthread_barrier_t barrier
  sem_t num_sleeping_workers
  bint is_done

cdef struct worker_t:
  scheduler_t *scheduler
  coro_switcher_t switcher
  coro_deque_t *deque
  pthread_t thread
  pthread_mutex_t mutex

cdef void scheduler_init(scheduler_t *scheduler, unsigned short num_workers=?) nogil

cdef void scheduler_coro_add(scheduler_t *scheduler, coro_function_t task, void *arg=?) nogil

cdef void scheduler_run(scheduler_t *scheduler) nogil

cdef void scheduler_destroy(scheduler_t *scheduler) nogil
 
cdef void worker_init(worker_t *worker, scheduler_t *scheduler) nogil

cdef void *worker_run(void *arg) nogil

cdef coro_t *worker_coro_get(worker_t *worker) nogil

cdef coro_t *worker_coro_steal(worker_t *thief) nogil

cdef void worker_destroy(worker_t *worker) nogil
