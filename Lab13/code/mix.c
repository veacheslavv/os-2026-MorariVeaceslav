// Lab 13: show interleaving without a lock, clean rows with a lock.
// Run as: mix          -> no protection, letters mix
//         mix 1        -> argv[1] = 1, use sem[0] as a mutex
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
row(char c)
{
  for (int i = 0; i < 20; i++)
    write(1, &c, 1);
  write(1, "\n", 1);
}

int
main(int argc, char *argv[])
{
  int use_lock = (argc > 1 && argv[1][0] == '1');

  if (use_lock)
    sem_init(0, 1);              // mutex: starts at 1

  int pid = fork();
  for (int k = 0; k < 3; k++) {
    if (use_lock) sem_down(0);
    row(pid == 0 ? 'b' : 'A');
    if (use_lock) sem_up(0);
  }
  if (pid != 0) wait(0);
  exit(0);
}
