// Lab 11: force an order between two threads with a signal semaphore.
// B reads a shared buffer AFTER A writes it.  The semaphore starts
// at 0 and is posted by A: sem_wait() blocks B until that post.
// If the starting value is changed to 1, B does not wait and reads
// an empty string.
//     gcc -Wall -pthread -o order order.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

char msg[32] = "";
sem_t ready;

void *
A(void *arg)
{
  sleep(1);                     // make sure B has time to block
  strcpy(msg, "hello");
  printf("A: wrote '%s'\n", msg);
  sem_post(&ready);
  return NULL;
}

void *
B(void *arg)
{
  printf("B: waiting...\n");
  sem_wait(&ready);
  printf("B: read '%s'\n", msg);
  return NULL;
}

int
main(void)
{
  sem_init(&ready, 0, 0);       // change this to 1 to break the order
  pthread_t a, b;
  pthread_create(&b, NULL, B, NULL);
  pthread_create(&a, NULL, A, NULL);
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  sem_destroy(&ready);
  return 0;
}
