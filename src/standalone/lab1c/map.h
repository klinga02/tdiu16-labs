/* do not forget the guard against multiple includes */
#include <stdbool.h>
#ifndef MAP_H
#define MAP_H

#define MAP_SIZE 32

typedef int key_t;    /* type of the key */
typedef char* value_t; /* type of the value */
typedef struct map map_t; /* type of the map */
typedef struct map* map_ptr_t; /* pointer to the map */

// struct map
// {
//   int size;         /* number of elements in the map */
//   key_t* keys;      /* array of keys */
//   value_t* values;  /* array of values */
// };

struct map
{
    value_t content[MAP_SIZE]; /* array of values */
};

void map_init(map_ptr_t m);

key_t map_insert(map_ptr_t m, value_t v);

value_t map_find(map_ptr_t m, key_t k);

value_t map_remove(map_ptr_t m, key_t k);

void map_for_each(map_ptr_t m, 
                void (*exec)(key_t k, value_t v, int aux), 
                int aux);

void map_remove_if(map_ptr_t m, 
                bool (*cond)(key_t k, value_t v, int aux), 
                int aux);

#endif 
