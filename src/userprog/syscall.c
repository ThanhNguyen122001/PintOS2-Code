#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);
void halt(void);
void exit(int stat);
pid_t exec(const char *thread_name);
int wait(pid_t pid);
bool create(const char *file_name, unsigned starting_size);
bool remove(const char *file_name);
int open(const char *file_name);
off_t fileSize(int fd);
int read(int fd, const void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell (int fd);
void close(int fd);


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
  // char *fn_copy;
  // int size;

  switch (f -> R.rax)
  {
  case SYS_HALT:
    printf("HALT!");
    halt();
    break;
  case SYS_EXIT:
    printf("EXIT!");
    int stat = *((int*) f -> esp + 1);
    exit(stat);
    break;
  case SYS_EXEC:
    printf("EXEC!");
    char *thread_name;
    int r_code = exec((const char *)thread_name);
    f -> eax = (uint32_t) r_code;
    break;
  case SYS_WAIT:
    printf("WAIT!");
    char *thread_name;
    int r_code = wait((const char *)thread_name);
    f -> eax = (uint32_t) r_code;
    break;
  case SYS_CREATE:
    printf("CREATE!");
    const char *file_name = ((const char*)f -> esp + 1);
    unsigned starting_size = *((unsigned *) f -> esp + 2);
    f -> eax = create(file_name, starting_size);
    break;
  case SYS_REMOVE:
    printf("REMOVE!");
    char *file_name = f -> eax + 1;
    f -> eax = remove(file_name);
    break;
  case SYS_OPEN:
    printf("OPEN!");
    char *file_name = f -> esp + 1;
    f -> eax = open(file_name);
    break;
  case SYS_FILESIZE:
    printf("FILESIZE!");
    int fd_size_file = *((int*)f -> esp + 1);
    f -> eax = fileSize(fd_size_file);
    break;
  case SYS_READ:
    printf("READ!");
    int *file_name = f -> esp + 1;
    char *buff = f -> esp + 2;
    unsigned *size = f -> esp + 3;
    f -> eax = read((int)file_name, (void*)buff, (unsigned)size);
    break;
  case SYS_WRITE:
    printf("WRITE!");
    // f -> R.rax = write(f -> R.rdi, f -> R.rsi, f -> R.rdx);
    break;
  case SYS_SEEK:
    printf("SEEK!");
    // seek(f -> R.rdi, f -> R.rsi);
    break;
  case SYS_TELL:
    printf("TELL!");
    // f -> R.rax = tell(f -> R.rdi);
    break;
  case SYS_CLOSE:
    printf("CLOSE!");
    // close(f -> R.rdi);
    break;
  default:
    exit(-1);
    break;
  }

  printf ("system call!\n");
  //thread_exit ();
}

void
halt(void){
  shutdown_power_off();
}

void 
exit (int status){
  struct thread *curr_thread = thread_current();
  thread_exit();
}

pid_t 
exec(const char *thread_name){
  pid_t id = process_execute(thread_name);
  return id;
}

int 
wait(pid_t pid){
  int id = process_wait(pid);
  return id;
}

bool 
create(const char *file_name, unsigned starting_size){
  bool created = false;

  if(file_name == NULL){
    return false;
  }else{
    created = filesys_create(file_name, starting_size);
    return created;
  }
  return created;
}

bool 
remove(const char *file_name){
  bool removed = false;

  if(file_name == NULL){
    return false;
  }else{
    removed = filesys_remove(file_name);
    return removed;
  }
}

int 
open(const char *file_name){
  struct file *open_file = filesys_open(file_name);
  
  if(open_file == NULL){
    return -1;
  }else{
    int fd = set_file(open_file);
    return fd;
  }
  return -1;
}

// off_t 
// fileSize(int fd){
//   struct process_file *curr_file = get_file(fd);
//   off_t size_file = 0; 

//   if(curr_file != NULL){
//     size_file = file_length(curr_file -> file);
//   }
//   return size_file;
// }

int 
read(int fd, const void *buffer, unsigned size){
  int size_byte = 0;
  check_address(buffer);
  struct thread *curr_thread = thread_current();
  struct file *curr_file = find_file_by_fd(fd);
  
  if(curr_file == NULL){
    return -1;
  }

  }

// void check_address(const uint64_t *pointer){
//   struct thread *curr_thread = thread_current();
//   if(pointer == NULL || )
// }
