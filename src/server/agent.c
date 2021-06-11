#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define MAX_PROC 25


typedef struct worker {
    char coroutines;
    pthread_mutex_t mutex;
    pthread_mutexattr_t mutexattr;
} worker_t;


typedef struct stopper {
    pthread_barrier_t barrier;
    pthread_barrierattr_t barattr;
} stopper_t;


void test_worker(int i, worker_t *workers[]) {
    for (int i = 0; i < MAX_PROC; i++){
        pthread_mutex_lock(&(workers[i]->mutex));
    }

    printf("\n %d Pegou \n", i);
    worker_t *curr;
    for(int i = 0; i < MAX_PROC; i++){
        curr = workers[i];
        printf("%c", curr->coroutines);
    }
    printf("\n");

    printf("\n %d Soltou \n", i);
    for (int i = MAX_PROC - 1; i >= 0; i--){
        pthread_mutex_unlock(&(workers[i]->mutex));
    }
}


int main() {
    stopper_t *stopper = create_shared_memory(sizeof(stopper_t));

    pthread_barrierattr_init(&stopper->barattr);
    pthread_barrierattr_setpshared(&stopper->barattr, PTHREAD_PROCESS_SHARED);
    pthread_barrier_init(&stopper->barrier, &stopper->barattr, MAX_PROC);

    char* alphabet = "abcdefghijklmnopqrstuvwxyz";
    worker_t *worker;
    worker_t *workers[MAX_PROC];
    for(int i = 0; i < MAX_PROC; i++){
        worker = create_shared_memory(sizeof(worker_t));
        worker->coroutines = *(alphabet+i);

        pthread_mutexattr_init(&worker->mutexattr);
        pthread_mutexattr_setpshared(&worker->mutexattr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&worker->mutex, &worker->mutexattr);

        workers[i] = worker;
    }

    int res;
    for(int i = 0; i < MAX_PROC; i++){
        res = fork();
        if (res == 0){
            test_worker(i, workers);
            pthread_barrier_wait(&stopper->barrier);
            break;
        }
    }

    if (res > 0){
        wait(NULL);

        for(int i = 0; i < MAX_PROC; i++){
            worker = workers[i];

            pthread_mutexattr_destroy(&worker->mutexattr);
            pthread_mutex_destroy(&worker->mutex);
            munmap(worker, sizeof(worker_t));
        }

        pthread_barrierattr_destroy(&stopper->barattr);
        pthread_barrier_destroy(&stopper->barrier);
    }

    return 0;
}