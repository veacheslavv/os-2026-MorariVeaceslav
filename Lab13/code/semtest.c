// Lab 13: xv6 semaphore test
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  // Part A: parent-child ordering via one semaphore (starts at 0).
  // Child waits on sem[0]; parent sleeps, then up()s. Child must print
  // AFTER parent, even though child is forked first.
  if (sem_init(0, 0) < 0) {
    printf("sem_init(0) failed\n");
    exit(1);
  }
  int pid = fork();
  if (pid == 0) {
    sem_down(0);
    printf("child:  I go after parent\n");
    exit(0);
  }
  pause(10);                    // give the child time to block
  printf("parent: I go first\n");
  sem_up(0);
  wait(0);

  // Part B: ping-pong between parent and child via two semaphores.
  // sem[1] starts at 1 (parent's turn), sem[2] starts at 0.
  sem_init(1, 1);
  sem_init(2, 0);
  pid = fork();
  if (pid == 0) {
    for (int i = 0; i < 3; i++) {
      sem_down(2);
      printf("  pong %d\n", i);
      sem_up(1);
    }
    exit(0);
  }
  for (int i = 0; i < 3; i++) {
    sem_down(1);
    printf("ping %d\n", i);
    sem_up(2);
  }
  wait(0);

  printf("semtest OK\n");
  exit(0);
}
