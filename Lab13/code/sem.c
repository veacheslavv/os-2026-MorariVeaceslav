// Lab 13: xv6 counting semaphores (kernel side).
// 8 slots, indexed 0..7. Each slot has an int value and a spinlock
// that protects the value.  P (down) blocks while value <= 0; V (up)
// increments the value and wakes one waiter.
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define NSEM 8

struct {
  struct spinlock lock;
  int value;
  int inited;
} sem[NSEM];

static int saninit = 0;

static void
sem_tab_init(void)
{
  if (saninit) return;
  for (int i = 0; i < NSEM; i++) {
    initlock(&sem[i].lock, "sem");
    sem[i].value = 0;
    sem[i].inited = 0;
  }
  saninit = 1;
}

// int sem_init(int idx, int value)
uint64
sys_sem_init(void)
{
  int idx, val;
  argint(0, &idx);
  argint(1, &val);
  if (idx < 0 || idx >= NSEM || val < 0)
    return -1;
  sem_tab_init();
  acquire(&sem[idx].lock);
  sem[idx].value = val;
  sem[idx].inited = 1;
  release(&sem[idx].lock);
  return 0;
}

// int sem_down(int idx)  --- P / wait
uint64
sys_sem_down(void)
{
  int idx;
  argint(0, &idx);
  if (idx < 0 || idx >= NSEM)
    return -1;
  sem_tab_init();
  acquire(&sem[idx].lock);
  if (!sem[idx].inited) {
    release(&sem[idx].lock);
    return -1;
  }
  while (sem[idx].value <= 0) {
    sleep_prepare(&sem[idx]);
    release(&sem[idx].lock);
    sleep();
    acquire(&sem[idx].lock);
  }
  sem[idx].value--;
  release(&sem[idx].lock);
  return 0;
}

// int sem_up(int idx)  --- V / signal
uint64
sys_sem_up(void)
{
  int idx;
  argint(0, &idx);
  if (idx < 0 || idx >= NSEM)
    return -1;
  sem_tab_init();
  acquire(&sem[idx].lock);
  if (!sem[idx].inited) {
    release(&sem[idx].lock);
    return -1;
  }
  sem[idx].value++;
  wakeup(&sem[idx]);
  release(&sem[idx].lock);
  return 0;
}
