// Lab 12: classic readers / writers with the first-reader-locks,
// last-reader-unlocks pattern.
//   * rlock protects the reader counter
//   * wlock is a binary semaphore that admits either one writer or
//     the whole reader group
// The first reader does sem_wait(&wlock), subsequent readers just
// bump the counter; the last reader does sem_post(&wlock).  Writers
// always take wlock exclusively.
//     gcc -Wall -pthread -o readers readers.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int data = 0;
int rcount = 0;
pthread_mutex_t rlock = PTHREAD_MUTEX_INITIALIZER;
sem_t wlock;

void *
reader(void *arg)
{
  int id = *(int *)arg;
  pthread_mutex_lock(&rlock);
  rcount++;
  if (rcount == 1) sem_wait(&wlock);
  pthread_mutex_unlock(&rlock);

  printf("reader %d reads %d   (readers inside: %d)\n", id, data, rcount);
  sleep(1);

  pthread_mutex_lock(&rlock);
  rcount--;
  if (rcount == 0) sem_post(&wlock);
  pthread_mutex_unlock(&rlock);
  return NULL;
}

void *
writer(void *arg)
{
  int *a = (int *)arg;
  int id = a[0], val = a[1];
  sem_wait(&wlock);
  data = val;
  printf("writer %d writes %d   (alone)\n", id, data);
  sleep(1);
  sem_post(&wlock);
  return NULL;
}

int
main(void)
{
  sem_init(&wlock, 0, 1);
  int w1[]  = {1, 10};
  int w2[]  = {2, 20};
  int r1 = 1, r2 = 2, r3 = 3;
  pthread_t tw1, tr1, tr2, tr3, tw2;

  pthread_create(&tw1, NULL, writer, w1);
  usleep(100000);               // make sure w1 goes first
  pthread_create(&tr1, NULL, reader, &r1);
  pthread_create(&tr2, NULL, reader, &r2);
  pthread_create(&tr3, NULL, reader, &r3);
  usleep(1500000);              // let all readers finish
  pthread_create(&tw2, NULL, writer, w2);

  pthread_join(tw1, NULL);
  pthread_join(tr1, NULL);
  pthread_join(tr2, NULL);
  pthread_join(tr3, NULL);
  pthread_join(tw2, NULL);
  sem_destroy(&wlock);
  return 0;
}
