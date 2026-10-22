#include "kernel/types.h"
#include "kernel/syscall.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 2){
    fprintf(2, "usage: sleep <ticks>\n");
    exit(1);
  }
#ifdef SYS_pause
  pause(atoi(argv[1]));   // system call into the kernel (new xv6 name)
#else
  sleep(atoi(argv[1]));   // system call into the kernel (older xv6 name)
#endif
  exit(0);
}
