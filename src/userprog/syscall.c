#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "lib/syscall-nr.h"
#include "lib/user/syscall.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "devices/input.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "userprog/process.h"

/* Process related: halt, exit, exec, wait
   File related: create, remove, open, filesize, read, write, seek, tell, close*/

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  lock_init(&file_lock);
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
    f -> eax = sys_wait(*(uint32_t*)(f -> esp + 4));
    break;
  case SYS_CREATE:
    addressChecker(f -> esp + 4);
    addressChecker(f -> esp + 8);
    f -> eax = create((char*)*(uint32_t*)(f -> esp + 4), 
      *(uint32_t*)(f -> esp + 8));
    break;
  case SYS_REMOVE:
    addressChecker(f -> esp + 4);
    f -> eax = remove((char*)*(uint32_t*)(f -> esp + 4));
    break;
  case SYS_OPEN:
    addressChecker(f -> esp + 4);
    f -> eax = open((char*)*(uint32_t*)(f -> esp + 4));
    break;
  case SYS_FILESIZE:
    addressChecker(f -> esp + 4);
    f -> eax = filesize(*(uint32_t*)(f -> esp + 4));
    break;
  case SYS_READ:
    addressChecker(f -> esp + 4);
    addressChecker(f -> esp + 8);
    addressChecker(f -> esp + 12);
    f -> eax = sys_read((int)*(uint32_t *)(f -> esp + 4), 
          (void*)*(uint32_t*)(f -> esp + 8), 
              (unsigned)*(uint32_t*)(f -> esp + 12));
    break;
  case SYS_WRITE:
    addressChecker(f -> esp + 4);
    addressChecker(f -> esp + 8);
    addressChecker(f -> esp + 12);
    f -> eax = sys_write((int)*(uint32_t *)(f -> esp + 4), 
          (void*)*(uint32_t*)(f -> esp + 8), 
              (unsigned)*(uint32_t*)(f -> esp + 12));
    break;
  case SYS_SEEK:
    addressChecker(f -> esp + 4);
    addressChecker(f -> esp + 8);
    seek((int)*(uint32_t*)(f -> esp + 4), (unsigned)*(uint32_t*)(f -> esp + 8));
    break;
  case SYS_TELL:
    addressChecker(f -> esp + 4);
    f -> eax = tell((int)*(uint32_t*)(f -> esp + 4));
    break;
  case SYS_CLOSE:
    addressChecker(f -> esp + 4);
    close(*(uint32_t*)(f -> esp + 4));
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
  tid_t tid;
  struct thread *curr_thread;

  tid = process_execute(cmd_line);
  curr_thread = get_c_process(tid);

  if(tid != -1){
    sema_down(&(get_c_process(tid) -> load_sema));
    if(curr_thread -> l_flag == false){
      return -1;
    }else{
      return tid;
    }
  }else{
    return -1;
  }
}

int sys_wait(pid_t pid){
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

int sys_write(int fd, const void* buffer, unsigned int size){
  int answer;
  struct file *curr_file;

  if(fd <= 0 || fd >= FD_SIZE){
    exit(-1);
  }

  addressChecker(buffer);
  lock_acquire(&file_lock);

  if(fd == 1){
    putbuf(buffer, size);
    return size;
  }else{
    curr_file = get_file(fd);
    if(curr_file == NULL){
      lock_release(&file_lock);
      exit(-1);
    }
    answer = file_write(curr_file, buffer, size);
    return answer;
  }
}

int sys_read(int fd, void *buffer, unsigned int size){
  int answer;
  uint8_t temp;

  if(fd < 0 || fd == 1 || fd >= FD_SIZE){
    exit(-1);
  }
  
  addressChecker(buffer);
  lock_acquire(&file_lock);

  if(fd == 0){
    for(answer = 0; (answer < size) && (temp = input_getc()); answer++){
      *(uint8_t*)(buffer + answer) = temp;
    }
  }else{
    struct file *curr_file = get_file(fd);
    if(curr_file == NULL){
      lock_release(&file_lock);
      exit(-1);
    }
    answer = file_read(curr_file, buffer, size);
  }
  lock_release(&file_lock);
  return answer;
}

int open(const char *file){
  int fd;
  struct file *curr_file;

  if(file == NULL){
    exit(-1);
  }

  lock_acquire(&file_lock);

  curr_file = filesys_open(file);

  if(curr_file == NULL){
    lock_release(&file_lock);
  }
  fd = add_file(curr_file);
  lock_release(file_lock);
  return fd;
}

int write(int fd, const void* buffer, unsigned int size){
  int file_answer;
  struct file *curr_file;

  if(fd <= 0 || fd > FD_SIZE){
    exit(-1);
  }

  lock_acquire(&file_lock);

  if(fd == 1){
    putbuf(buffer, size);
    lock_release(&file_lock);
    return size;
  }else{
    curr_file = get_file(fd);
    if(curr_file == NULL){
      lock_release(&file_lock);
      exit(-1);
    }
    file_answer = file_write(curr_file, buffer, size);
    lock_release(&file_lock);
    return file_answer;
  }
}

bool create(const char *file, unsigned inital_size){
  if(file == NULL){
    exit(-1);
  }
  return filesys_create(file, inital_size);
}

bool remove(const char *file){
  if(file == NULL){
    exit(-1);
  }
  return filesys_remove(file);
}

void close(int fd){
  close_file(fd);
}

int filesize(int fd){
  struct file *curr_file = get_file(fd);
  if(curr_file == NULL){
    exit(-1);
  }
  return file_length(curr_file);
}

void seek(int fd, unsigned int position){
  struct file *curr_file = get_file(fd);
  
  if(curr_file == NULL){
    exit(-1);
  }
  file_seek(curr_file, position);
}

unsigned int tell(int fd){
  struct file *curr_file = get_file(fd);
  if(curr_file == NULL){
    exit(-1);
  }
  return file_tell(curr_file);
}

struct file *get_file(int fd){
  struct thread *curr_thread = thread_current();

  if(fd < 3 || fd >= FD_SIZE){
    return NULL;
  }
  return curr_thread -> file_desc_list[fd];
}

int add_file(struct file* file){
  struct thread *curr_thread = thread_current();
  int i;
  
  for(i = 3; i < FD_SIZE; i++){
    if(curr_thread -> file_desc_list[i] == NULL){
      curr_thread -> file_desc_list[i] = file;
      return i;
    }
  }
  return -1;
}

void close_file(int fd){
  struct thread *curr_thread = thread_current();
  
  if((fd < 3) || (fd >= FD_SIZE)){
    return;
  }

  if(curr_thread -> file_desc_list[fd] != NULL){
    file_close(curr_thread -> file_desc_list[fd]);
    curr_thread -> file_desc_list[fd] = NULL;
  }
}