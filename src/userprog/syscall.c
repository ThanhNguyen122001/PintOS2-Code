#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "lib/syscall-nr.h"
#include "lib/user/syscall.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "devices/input.h"

/* Process related: halt, exit, exec, wait
   File related: create, remove, open, filesize, read, write, seek, tell, close*/

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void addressChecker(void* address){
  struct thread *curr_thread = thread_current();
  if(address == NULL){
    exit(-1);
  }
  if(!is_user_vaddr(address)){
    exit(-1);
  }
  if(!pagedir_get_page(curr_thread -> pagedir, address) == NULL){
    exit(-1);
  }
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  switch(*(int32_t *)(f -> esp))
  {
  case SYS_HALT:
    shutdown_power_off();
    break;
  case SYS_EXIT:
    addressChecker(f -> esp + 4);
    exit(*(int*)(f -> esp + 4));
    break;
  case SYS_EXEC:
    addressChecker(f -> esp + 4);
    f -> eax = exec((char*)*(uint32_t*)(f -> esp + 4));
    break;
  case SYS_WAIT:
    addressChecker(f -> esp + 4);
    f -> eax = wait(*(uint32_t*)(f -> esp + 4));
    break;
  case SYS_CREATE:
    break;
  case SYS_REMOVE:
    break;
  case SYS_OPEN:
    break;
  case SYS_FILESIZE:
    break;
  case SYS_READ:
    addressChecker(f -> esp + 4);
    addressChecker(f -> esp + 8);
    addressChecker(f -> esp + 12);
    f -> eax = read((int)*(uint32_t *)(f -> esp + 4), 
          (void*)*(uint32_t*)(f -> esp + 8), 
              (unsigned)*(uint32_t*)(f -> esp + 12));
    break;
  case SYS_WRITE:
    addressChecker(f -> esp + 4);
    addressChecker(f -> esp + 8);
    addressChecker(f -> esp + 12);
    f -> eax = write((int)*(uint32_t *)(f -> esp + 4), 
          (void*)*(uint32_t*)(f -> esp + 8), 
              (unsigned)*(uint32_t*)(f -> esp + 12));
    break;
  case SYS_SEEK:
    break;
  case SYS_TELL:
    break;
  case SYS_CLOSE:
    break;
  default:
    exit(-1);
  } 

  printf ("system call!\n");
  thread_exit ();
}


void exit(int status){
  struct thread *curr_thread = thread_current();
  printf("%s: exit(%d)\n" , curr_thread -> name , status);
  curr_thread -> exit_status = status;
  thread_exit();
}

pid_t exec(const char *cmd_line){
  process_execute(cmd_line);
}

int wait(pid_t pid){
  process_wait(pid);
}

int read(int fd, void *buffer, unsigned int size){
  unsigned int a;
  uint8_t t;

  if(fd == 0){
    a = 0;
    for(a = 0; (a < size) && (t = input_getc()); a++){
      *(uint8_t*)(buffer + a) = t;
    }
    return a;
  }else{
    return -1;
  }
}

int write(int fd, const void* buffer, unsigned int size){
  if(fd == 1){
    putbuf(buffer, size);
    return size;
  }
  return -1;
}

