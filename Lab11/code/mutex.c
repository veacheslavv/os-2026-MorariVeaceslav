// Lab 11: fix the race with a mutex.
// Only one thread can hold the mutex at a time, so the three steps
// of `counter++` always happen without interleaving.  The result
// is now always 2*N.
//     gcc -Wall -pthread -o mutex mutex.c
#include <stdio.h>
#include <pthread.h>

#define N 1000000L

long counter = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *
worker(void *arg)
{
  for (long i = 0; i < N; i++) {
    pthread_mutex_lock(&m);
    counter++;
    pthread_mutex_unlock(&m);
  }
  return NULL;
}

int
main(void)
{
  pthread_t a, b;
  pthread_create(&a, NULL, worker, NULL);
  pthread_create(&b, NULL, worker, NULL);
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  printf("counter = %ld   (expected %ld)\n", counter, 2 * N);
  return 0;
}
