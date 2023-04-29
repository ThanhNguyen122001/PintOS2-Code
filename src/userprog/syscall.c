#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);
void halt(void);
void exit(int status);
pid_t exec (const char *cmd_line);
int wait (pid_t pid);



/* 
 Process related: halt, exit, exec, wait
 File related: create, remove, open, filesize, read, write, seek, tell, close
*/

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
    halt();
    break;
  case SYS_EXIT:
    int status = (int)(*(uint32_t*)(f -> esp + 4));
    exit(status);
    f -> eax = (int)(*(uint32_t*)(f -> esp + 4));
    break;
  case SYS_EXEC:
    char *file_name =(char*)*(uint32_t*)(f -> esp + 4);
    f -> eax = exec(file_name);
    break;
  case SYS_WAIT:
    pid_t childPid = (pid_t)*(uint32_t*)(f -> esp + 4);
    f -> eax = wait(childPid);
    break;
}
}

void halt(void){
  shutdown_power_off();
}

void exit(int status){
  struct thread *curr_thread = thread_current();
  printf("Name of process: %d", status);
  thread_exit();
}

pid_t exec(const char *cmd_line){
  return process_execute(cmd_line);
}

int wait(pid_t pid){
  return process_wait(pid);
}