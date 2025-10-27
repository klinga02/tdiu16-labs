#ifndef PLIST_H
#define PLIST_H


/* Place functions to handle a running process here (process list).

   plist.h : Your function declarations and documentation.
   plist.c : Your implementation.

   The following is strongly recommended:

   - A function that given process inforamtion (up to you to create)
     inserts this in a list of running processes and return an integer
     that can be used to find the information later on.

   - A function that given an integer (obtained from above function)
     FIND the process information in the list. Should return some
     failure code if no process matching the integer is in the list.
     Or, optionally, several functions to access any information of a
     particular process that you currently need.

   - A function that given an integer REMOVE the process information
     from the list. Should only remove the information when no process
     or thread need it anymore, but must guarantee it is always
     removed EVENTUALLY.

   - A function that print the entire content of the list in a nice,
     clean, readable format.

 */

#include <stdbool.h>
#include "threads/synch.h"
#include "threads/thread.h"


#define PLIST_SIZE 100

typedef int key_t;    /* type of the key */
typedef struct process* value_ptr_t; /* type of the value */
typedef struct plist plist_t; /* type of the plist */
typedef struct plist* plist_ptr_t; /* pointer to the plist */
typedef int tid_t;


struct process
{
  tid_t tid; // kan inte includa os.h och någon debug.h samtidigt 
  tid_t parentid;     
  int status;
  //bool crashed;     
  // struct process *parent; // detsamma som med children list behöver detta bara vara
  // struct list children;   // ett idx till plist för alla möjliga processer är där
  struct semaphore sema;   
  bool alive;              
  bool parent_alive;              
  bool is_waited_on;         
};

struct plist
{
  value_ptr_t content[PLIST_SIZE];
};

bool new_process_init(tid_t t, int parentid);

// void process_insert(value_ptr_t p);

void plist_init(void); 

key_t plist_insert(value_ptr_t v);

value_ptr_t plist_find(tid_t t);

void plist_remove(key_t k);

// bool do_free (key_t kvalue_ptr_t vint aux);

void plist_for_each(void (*exec)(key_t k, value_ptr_t v, int aux), 
                    int aux);

// void plist_remove_all(void);

void plist_print(void);

#endif