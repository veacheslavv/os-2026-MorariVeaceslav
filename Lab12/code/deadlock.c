// Lab 12: a textbook deadlock caused by circular lock order.
// Thread one takes A then B; thread two takes B then A.  With the
// sleep(1) in the middle, both threads are guaranteed to hold their
// first lock before asking for the second, so each is waiting for
// the lock the other holds.  timeout kills the program after 5 s
// with exit code 124.
//
// The fix (see deadlock_fixed.c): make BOTH threads take A first,
// then B.  No circular wait is possible.
//     gcc -Wall -pthread -o deadlock deadlock.c
//     timeout 5 ./deadlock ; echo "exit code: $?"      # 124
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t B = PTHREAD_MUTEX_INITIALIZER;

void *
one(void *arg)
{
  pthread_mutex_lock(&A);
  printf("one: I have A, I want B\n");
  sleep(1);
  pthread_mutex_lock(&B);               /* the trap */
  printf("one: I have both\n");
  pthread_mutex_unlock(&A);
  pthread_mutex_unlock(&B);
  return NULL;
}

void *
two(void *arg)
{
  pthread_mutex_lock(&B);
  printf("two: I have B, I want A\n");
  sleep(1);
  pthread_mutex_lock(&A);               /* waits for one */
  printf("two: I have both\n");
  pthread_mutex_unlock(&A);
  pthread_mutex_unlock(&B);
  return NULL;
}

int
main(void)
{
  pthread_t t1, t2;
  pthread_create(&t1, NULL, one, NULL);
  pthread_create(&t2, NULL, two, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  printf("done\n");
  return 0;
}
