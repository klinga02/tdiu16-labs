#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

/* header files you probably need, they are not used yet */
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"
#include "lib/user/syscall.h"

#include "devices/timer.h"

static void syscall_handler(struct intr_frame *);

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}


bool verify_fix_length(void* start, unsigned length)
{
  if (is_kernel_vaddr(start) || is_kernel_vaddr((void*)((unsigned)start + length - 1)))
  {
    return false;
  }

  unsigned end = pg_no(pg_round_down((void*)((unsigned)start + length - 1)));
  
  for (unsigned i = pg_no(pg_round_down(start)); i <= end; i++)
  {
    if (pagedir_get_page(thread_current()->pagedir, (void*)(i*PGSIZE)) == NULL)   
    {
      return false;
    }
  }
  return true;
}


bool verify_variable_length(char* start)
{
  const char* addr = start;
  unsigned current_page = -1;
 
  
  while (true)
  {
    if (current_page != pg_no(addr))
    {
      current_page = pg_no(addr);
      if ( is_kernel_vaddr(addr) || pagedir_get_page(thread_current()->pagedir, addr) == NULL)   
      {
        return false;
      }
    }
    
    if (*addr == '\0'){
      return true;
    }
    
    addr++;
  }
}

/* This array defined the number of arguments each syscall expects.
   For example, if you want to find out the number of arguments for
   the read system call you shall write:

   int sys_read_arg_count = argc[ SYS_READ ];

   All system calls have a name such as SYS_READ defined as an enum
   type, see `lib/syscall-nr.h'. Use them instead of numbers.
 */
const int argc[] = {
    /* basic calls */
    0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1,
    /* not implemented */
    2, 1, 1, 1, 2, 1, 1,
    /* extended, you may need to change the order of these two (plist, sleep) */
    0, 1};

static void
syscall_handler(struct intr_frame *f)
{
  int32_t *esp = (int32_t *)f->esp;
  
  if (!verify_fix_length(esp, sizeof(int32_t))) {
    process_exit(-1);
  }

  if (esp[0] > SYS_NUMBER_OF_CALLS || esp[0] < 0)
  {
    process_exit(-1);
  }
  int syscall_number = esp[0];
  int arg_count = argc[syscall_number];


  if(!verify_fix_length((void*)esp, sizeof(void*) * (argc[syscall_number] + 1))){
    process_exit(-1);
  }

  int arg1 = (arg_count > 0) ? esp[1] : 0;
  int arg2 = (arg_count > 1) ? esp[2] : 0;
  int arg3 = (arg_count > 2) ? esp[3] : 0;

 

  switch (syscall_number)
  {
  case SYS_HALT: //void
  {
    power_off();
    break;
  }

  case SYS_EXIT: // status/ return value
  {
    process_exit(arg1);
    break;
  }
  case SYS_EXEC: // file*
  {

    char *file_name = (char *)arg1;
    if (file_name == NULL){
      process_exit(-1);
    }
    if (!verify_variable_length(file_name)) {
      process_exit(-1);
    }

    f->eax = process_execute(file_name);
    break;
  }
  case SYS_WAIT: // id
  {
    f->eax = process_wait(arg1);
    break;
  }
  case SYS_READ: // SYS_READ, fd, buffer, size
  {
    if (!verify_fix_length((void *)arg2, arg3)) {
      process_exit(-1);
    }
    if (arg1 == STDIN_FILENO)
    {
      for (int i = 0; i < arg3; i++)
      {
        char in_put = input_getc(); // src/devices/input.h
        if (in_put == '\r')
          in_put = '\n';
        ((char *)arg2)[i] = in_put;
        printf("%c", in_put);
      }
      f->eax = arg3; // ret the number of bytes read
    }
    else
    {
      // char *file_name = (char *)arg1;
      // if (file_name == NULL){
      //   process_exit(-1);
      // }
      // if (!verify_variable_length(file_name)) {
      //   process_exit(-1);
      // }
      struct file *file = map_find(&thread_current()->open_files, arg1);
      if (file == NULL)
      {
        f->eax = -1;
      }
      else
      {
        off_t bytes_read = file_read(file, (void *)arg2, arg3);
        f->eax = bytes_read;
      }
    }
    break;
  }
  case SYS_WRITE: // SYS_WRITE, fd, buffer, size
  {
    if (!verify_fix_length((void *)arg2, arg3)) {
      process_exit(-1);
    }
    if (arg1 == STDOUT_FILENO)
    {
      putbuf((char *)arg2, arg3);
    }
    else
    {
      // char *file_name = (char *)arg1;
      // if (file_name == NULL){
      //   process_exit(-1);
      // }
      // if (!verify_variable_length(file_name)) {
      //   process_exit(-1);
      // }

      struct file *file = map_find(&thread_current()->open_files, arg1);
      if (file != NULL)
      {
        f->eax = file_write(file, (char *)arg2, arg3);
      }
      else
      {
        f->eax = -1;
      }
    }
    break;
  }
  case SYS_CREATE:  // const char *file, unsigned initial_size
  {
    char *file_name = (char *)arg1;
    if (file_name == NULL){
      process_exit(-1);
    }
    if (!verify_variable_length(file_name)) {
      process_exit(-1);
    }

    unsigned initial_size = arg2;
    bool success = filesys_create(file_name, initial_size);
    f->eax = success;
    break;
  }
  case SYS_REMOVE:  // const char *file
  {
    char *file_name = (char *)arg1;
    bool success = filesys_remove(file_name);
    f->eax = success;
    break;
  }
  case SYS_OPEN:  // const char *file
  {
    char *file_name = (char *)arg1;

    if (file_name == NULL)
      process_exit(-1);

    if (!verify_variable_length(file_name)) {
      process_exit(-1);
    }
    struct file *file = filesys_open(file_name);
    if (file == NULL)
    {
      f->eax = -1;
    }
    else
    {
      int fd = map_insert(&thread_current()->open_files, file);
      f->eax = fd;
    } // returnera fd-"idx" enligt sida 28 wiki 'int open()'
    break;
  }
  case SYS_CLOSE: // int fd
  {
    int fd = arg1;
    struct file *file = map_find(&thread_current()->open_files, fd);
    if (file == NULL)
    {
      f->eax = -1;
    }
    else
    {
      map_remove(&thread_current()->open_files, fd);
      f->eax = 0;
    }
    break;
  }
  case SYS_FILESIZE:  // int fd
  {
    int fd = arg1;
    struct file *file = map_find(&thread_current()->open_files, fd);
    if (file == NULL)
    {
      f->eax = -1;
    }
    else
    {
      f->eax = file_length(file);
    }
    break;
  }
  case SYS_SEEK:  // int fd, unsigned position
  {
    int fd = arg1;
    unsigned pos = arg2;
    struct file *file = map_find(&thread_current()->open_files, fd);
    if (file == NULL)
    {
      f->eax = -1;
    }
    else
    {
      off_t file_size = file_length(file);
      if (pos > (unsigned)file_size)
      {
        pos = file_size;
      }
      file_seek(file, pos);
      f->eax = 0;
    }
    break;
  }
  case SYS_TELL:  // int fd
  {
    int fd = arg1;
    struct file *file = map_find(&thread_current()->open_files, fd);
    f->eax = file_tell(file);
    break;
  }
  case SYS_PLIST: // void
  {
    plist_print();
    break;
  }

  case SYS_SLEEP: // int millis
  {
    timer_msleep(arg1);
    break;
  }

  default:
  {
    printf("Executed an unknown system call!\n");

    printf("Stack top + 0: %d\n", esp[0]);
    printf("Stack top + 1: %d\n", esp[1]);

    thread_exit();
  }
  }
}
