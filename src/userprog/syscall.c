#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "process.h"
#include "filesys/filesys.h"
#include "threads/malloc.h"
#include "lib/user/syscall.h"
#include "threads/init.h"
#include "threads/flags.h"



static void syscall_handler (struct intr_frame *);
void halt(void);
void exit(int status);
pid_t exec (const char *cmd_line);
int wait (pid_t pid);



/* 
 Process related: halt, exit, exec, wait
 File related: create, remove, open, filesize, read, write, seek, tell, close
*/

int status;
char *file_name;
pid_t childPid;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  
  uint32_t *p = f -> esp;

  switch (*p)
  {
  case SYS_HALT:
    printf("HALT!");
    halt();
    break;
  case SYS_EXIT:
    printf("EXIT!");
    status = (int)(*(uint32_t*)(f -> esp + 4));
    exit(status);
    f -> eax = (int)(*(uint32_t*)(f -> esp + 4));
    break;
  case SYS_EXEC:
    printf("EXEC!");
    file_name =(char*)*(uint32_t*)(f -> esp + 4);
    f -> eax = exec(file_name);
    break;
  case SYS_WAIT:
    printf("WAIT");
    childPid = (pid_t)*(uint32_t*)(f -> esp + 4);
    f -> eax = wait(childPid);
    break;
  default:
    exit(-1);
    break;
  }
}

void halt(void){
  shutdown_power_off();
}

void exit(int status){
  struct thread *curr_thread = thread_current();
  printf("Name of process: %s | Exit: %d", curr_thread -> name,status);
  thread_exit();
}

pid_t exec(const char *cmd_line){
  return process_execute(cmd_line);
}

int wait(pid_t pid){
  return process_wait(pid);
}