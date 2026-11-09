// Lab 11: show a data race.
// Two threads each increment the same counter N times with no
// protection.  The expected value is 2*N; the printed value is
// almost always less, because `counter++` is really three steps
// (load, add, store) and the two threads overwrite each other.
// Compile without -O2 to keep the race visible:
//     gcc -Wall -pthread -O0 -o race race.c
#include <stdio.h>
#include <pthread.h>

#define N 1000000L

long counter = 0;

void *
worker(void *arg)
{
  for (long i = 0; i < N; i++)
    counter++;
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
