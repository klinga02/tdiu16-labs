#include <stddef.h>


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "plist.h"
#include "../filesys/file.h"
#include "threads/malloc.h"
#include "threads/synch.h"

struct plist global_plist;
struct lock plist_lock;

bool new_process_init(tid_t t, int processid)
{
    value_ptr_t p = malloc(sizeof(struct process));
    if (p == NULL)
    {
        //fprintf(stderr, "Error: Memory allocation failed in process_init\n");
        //debug("Error: Memory allocation failed in process_init\n");
        return false;
    }
    p->tid = t;
    p->parentid = processid;
    
    p->status = -1;

    sema_init(&p->sema, 0); 
    p->alive = true;
    p->parent_alive = true;
   

    p->is_waited_on = false;
    int success = plist_insert(p);
    if ( success == -1)
    {
        free(p);
        return false;
    } // insert the process into the global plist
    return true;
}

void plist_init()
{
    for (int i = 0; i < PLIST_SIZE; i++)
    {
        global_plist.content[i] = NULL; // init all pointers to NULL
    }
    lock_init(&plist_lock);
}


key_t plist_insert(value_ptr_t v)
{
    int result = -1;
    lock_acquire(&plist_lock);
    for(int i = 0; i < PLIST_SIZE; i++)
    {
        
        if(global_plist.content[i] == NULL)
        {
            global_plist.content[i] = v;
            result = i;
            break;
        }
        else if(global_plist.content[i] == v)
        {
            break;
        }
    }
    lock_release(&plist_lock);
    return result;
}

value_ptr_t plist_find(tid_t t)
{
    value_ptr_t result = NULL;
    lock_acquire(&plist_lock);
    for (int i = 0; i < PLIST_SIZE; i++)
    {
        if (global_plist.content[i] != NULL && global_plist.content[i]->tid == t){
            result = global_plist.content[i]; // ret the value at the given idx
        }
    }
    lock_release(&plist_lock);
    return result; // invalid key

}

void plist_remove(tid_t t)
{
    value_ptr_t p = plist_find(t);
    
    lock_acquire(&plist_lock);
    if (p != NULL)
    {
        for (int i = 0; i < PLIST_SIZE; i++)
        {
            if (global_plist.content[i] != NULL && global_plist.content[i]->tid == t)
            {
                free(p); 
                global_plist.content[i] = NULL;
                break;
            }
        }
    }
    lock_release(&plist_lock);
}


void plist_print()
{
    int count = 0;
    lock_acquire(&plist_lock);
    for ( int i = 0; i < PLIST_SIZE; i++)
    {
        if(global_plist.content[i] != NULL)
        {
            struct process* p = global_plist.content[i];
            debug("Process ID: %-5d  Parent ID: %-5d  Alive: %-3s  ParentAlive: %-3s  Status: %-3d\n",
                p->tid,
                p->parentid,
                p->alive ? "Yes" : "No",
                p->parent_alive ? "Yes" : "No",
                p->status);
            count++;
        }
    }
    lock_release(&plist_lock);
    if (count > 5)
        debug("Count: %d\n", count);

}
