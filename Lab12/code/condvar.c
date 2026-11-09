// Lab 12: force an order with a condition variable instead of a
// signal semaphore.  The condition variable always goes with a
// mutex: pthread_cond_wait() atomically releases the mutex and
// sleeps, then reacquires it on wake.  The waiting side uses
// while (!ready) and not if, to tolerate spurious wakeups.
//     gcc -Wall -pthread -o condvar condvar.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

char msg[32] = "";
int ready = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

void *
A(void *arg)
{
  sleep(1);                     // give B time to go to sleep first
  pthread_mutex_lock(&m);
  strcpy(msg, "hello");
  ready = 1;
  printf("A: message written\n");
  pthread_cond_signal(&cv);
  pthread_mutex_unlock(&m);
  return NULL;
}

void *
B(void *arg)
{
  pthread_mutex_lock(&m);
  while (!ready) {
    printf("B: not ready, I sleep\n");
    pthread_cond_wait(&cv, &m);
  }
  printf("B: I read '%s'\n", msg);
  pthread_mutex_unlock(&m);
  return NULL;
}

int
main(void)
{
  pthread_t a, b;
  pthread_create(&b, NULL, B, NULL);
  usleep(100000);               // let B reach pthread_cond_wait
  pthread_create(&a, NULL, A, NULL);
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  return 0;
}
