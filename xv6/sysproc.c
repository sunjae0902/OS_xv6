#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#define NULL ((void *)0)


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
int
sys_forknexec(void)
{
    char *path;
    char **args=NULL; // 문자열 포인터 배열, +1은 널 포인터를 위한 공간
   
    
    // 첫 번째 인자인 path 추출
    if (argstr(0, &path) < 0) {
        cprintf("경로 추출 실패\n");
       return -1;
    }
    
    if(argptr(1, (void *)&args, sizeof(args[0]))) {
        cprintf("인자 추출 실패\n");
       return -1;
    }
   

    // forknexec 함수 호출
   
    return forknexec(path,(const char **) args);
}

int
sys_set_proc_priority(void){
    
   int pid, priority;
    
    if(argint(0, &pid) < 0)
        return -1;
    if(argint(1,&priority) < 0)
        return -1;
    
    return set_proc_priority(pid, priority);

}

int
sys_get_proc_priority(void){
    
     int pid;
     
     if(argint(0, &pid) < 0)
         return -1;
  
     return get_proc_priority(pid);

    
}

int sys_getNumFreePages(void){
    return getNumFreePages();
}

