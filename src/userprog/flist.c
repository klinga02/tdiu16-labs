#include <stddef.h>

#include "flist.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../filesys/file.h"


void map_init(map_ptr_t m)
{
    for (int i = 0; i < MAP_SIZE; i++)
    {
        m->content[i] = NULL; // init all pointers to NULL
    }
}


key_t map_insert(map_ptr_t m, value_t v)
{
    int i = 2;
    while (m->content[i] != NULL && i < MAP_SIZE)
    {
        i++;
    }
    if (i == MAP_SIZE)
    {
        // fprintf(stderr, "Error: Map is full\n");
        return -1; // map is full
    }
    m->content[i] = v;
    return i; // ret the idx of the inserted value
}

value_t map_find(map_ptr_t m, key_t k)
{
    if (k < 0 || k >= MAP_SIZE)
    {
        return NULL; // invalid key
    }
    return m->content[k]; // ret the value at the given idx
}

value_t map_remove(map_ptr_t m, key_t k)
{
    value_t rmv = m->content[k]; 
    if (k < 0 || k >= MAP_SIZE)
    {
        return NULL; // invalid key
    }
    file_close(m->content[k]); 
    m->content[k] = NULL; 
    return rmv; // ret NULL to indicate that the value has been removed

}


void map_for_each(map_ptr_t m, 
                void (*exec)(key_t k, value_t v, int aux), 
                int aux)
{   
    for (int j = 0; j < MAP_SIZE; j++)
    {
        if (m->content[j] != NULL)
        {
            exec(j, m->content[j], aux); // key, value,  aux
        }
    }
}

void map_remove_if(map_ptr_t m)
{  
    for (int j = 0; j < MAP_SIZE; j++)
    {
        if( m->content[j] != NULL)
        {
            file_close(m->content[j]); 
            m->content[j] = NULL; 
        }
    }
}
