// Lab 11: fix the same race with a binary semaphore.
// sem initialised to 1 behaves exactly like a mutex: sem_wait()
// is P (decrement; block if 0), sem_post() is V (increment).
//     gcc -Wall -pthread -o sem sem.c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N 1000000L

long counter = 0;
sem_t s;

void *
worker(void *arg)
{
  for (long i = 0; i < N; i++) {
    sem_wait(&s);
    counter++;
    sem_post(&s);
  }
  return NULL;
}

int
main(void)
{
  sem_init(&s, 0, 1);
  pthread_t a, b;
  pthread_create(&a, NULL, worker, NULL);
  pthread_create(&b, NULL, worker, NULL);
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  printf("counter = %ld   (expected %ld)\n", counter, 2 * N);
  sem_destroy(&s);
  return 0;
}
