// Lab 12: classic producer / consumer with a bounded buffer.
// Three primitives work together:
//   * empty semaphore counts free slots  (starts at SIZE)
//   * full  semaphore counts filled slots (starts at 0)
//   * mutex protects the ring buffer indices
// Producer runs faster than consumer, so the buffer fills to 4 and
// the producer blocks until the consumer makes room.
//     gcc -Wall -pthread -o prodcons prodcons.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 4
#define N    8

int buf[SIZE];
int head = 0, tail = 0, count = 0;
sem_t empty, full;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *
producer(void *arg)
{
  for (int i = 1; i <= N; i++) {
    sem_wait(&empty);
    pthread_mutex_lock(&m);
    buf[tail] = i;
    tail = (tail + 1) % SIZE;
    count++;
    printf("put %d    (in buffer: %d)\n", i, count);
    pthread_mutex_unlock(&m);
    sem_post(&full);
    usleep(100000);             // 0.1 s
  }
  return NULL;
}

void *
consumer(void *arg)
{
  for (int i = 1; i <= N; i++) {
    sem_wait(&full);
    pthread_mutex_lock(&m);
    int v = buf[head];
    head = (head + 1) % SIZE;
    count--;
    printf("           take %d    (in buffer: %d)\n", v, count);
    pthread_mutex_unlock(&m);
    sem_post(&empty);
    usleep(300000);             // 0.3 s
  }
  return NULL;
}

int
main(void)
{
  sem_init(&empty, 0, SIZE);
  sem_init(&full,  0, 0);
  pthread_t p, c;
  pthread_create(&p, NULL, producer, NULL);
  pthread_create(&c, NULL, consumer, NULL);
  pthread_join(p, NULL);
  pthread_join(c, NULL);
  sem_destroy(&empty);
  sem_destroy(&full);
  return 0;
}
