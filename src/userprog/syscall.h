#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

//System Call
void halt(void);
void exit(int status);
pid_t exec (const char *cmd_line);
int wait (pid_t pid);
pid_t exec(const *cmd_line);
void exit (int status);

//File Manipulation
bool create(const char *file, unsigned initial_size);
bool remove(const char *file);
int open(const char *file);
int filesize(int fd);
int read(int fd, void *buffer, unsigned size);

//Etc.


#endif /* userprog/syscall.h */
