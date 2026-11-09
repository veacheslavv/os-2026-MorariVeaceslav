// Lab 11: a counting semaphore as a resource counter.
// The parking lot has 3 free spaces; 6 cars want to park.
// sem_wait(&spaces) is "take a free space", sem_post is "release
// one".  The number of cars inside is tracked separately to prove
// it never exceeds 3.
//     gcc -Wall -pthread -o parking parking.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define CARS    6
#define PLACES  3

sem_t spaces;
int inside = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *
car(void *arg)
{
  int id = *(int *)arg;
  printf("car %d: waiting for a space\n", id);
  sem_wait(&spaces);

  pthread_mutex_lock(&m);
  inside++;
  printf("car %d: parked           (inside: %d)\n", id, inside);
  pthread_mutex_unlock(&m);

  sleep(1);                     // "shopping"

  pthread_mutex_lock(&m);
  inside--;
  printf("car %d: leaving          (inside: %d)\n", id, inside);
  pthread_mutex_unlock(&m);

  sem_post(&spaces);
  return NULL;
}

int
main(void)
{
  sem_init(&spaces, 0, PLACES);
  pthread_t t[CARS];
  int ids[CARS];
  for (int i = 0; i < CARS; i++) {
    ids[i] = i + 1;
    pthread_create(&t[i], NULL, car, &ids[i]);
  }
  for (int i = 0; i < CARS; i++)
    pthread_join(t[i], NULL);
  sem_destroy(&spaces);
  return 0;
}
