// Lab 12: deadlock fixed by lock ordering.
// BOTH threads now take A first, then B.  No circular wait can
// form: the thread that gets A can always get B next because
// nobody else holds B without first having A.
//     gcc -Wall -pthread -o deadlock_fixed deadlock_fixed.c
//     timeout 5 ./deadlock_fixed ; echo "exit code: $?"      # 0
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
  pthread_mutex_lock(&B);               // FIXED: still A then B
  printf("one: I have both\n");
  pthread_mutex_unlock(&B);
  pthread_mutex_unlock(&A);
  return NULL;
}

void *
two(void *arg)
{
  pthread_mutex_lock(&A);               // FIXED: lock A first
  printf("two: I have B, I want A\n");  // kept the old message
  sleep(1);
  pthread_mutex_lock(&B);               // FIXED: then B
  printf("two: I have both\n");
  pthread_mutex_unlock(&B);
  pthread_mutex_unlock(&A);
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
