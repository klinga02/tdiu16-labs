#include <syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "tests/lib.h"
#include "tests/main.h"

/**
 * From threads/vaddr.h:
 */
#define BITMASK(SHIFT, CNT) (((1ul << (CNT)) - 1) << (SHIFT))

#define PGSHIFT 0                          /* Index of first offset bit. */
#define PGBITS  12                         /* Number of offset bits. */
#define PGSIZE  (1 << PGBITS)              /* Bytes in a page. */
#define PGMASK  BITMASK(PGSHIFT, PGBITS)   /* Page offset bits (0:12). */

static inline void *pg_round_up (const void *va) {
    return (void *) (((uintptr_t) va + PGSIZE - 1) & ~PGMASK);
}

#define PHYS_BASE ((void *)0xC0000000)

const char filename[] = "f";
char stack_backup[2];

void test_main(void)
{
  if (!create(filename, 4)) {
    fail("Failed to create a file!");
  }

  int fd = open(filename);
  if (fd < 0) {
    fail("Failed to open the created file!");
  }
  close(fd);

  // Call open() with a string placed right before PHYS_BASE, but without a terminator:
  int len = strlen(filename);
  char *copy_start = PHYS_BASE;
  copy_start -= len;

  // Note: We will be overwriting parts of the stack. We need to restore it afterwards.
  for (int i = 0; i < len; i++) {
    stack_backup[i] = copy_start[i];
    copy_start[i] = filename[i];
  }

  // Call open with a non-null-terminated string. This should fail.
  fd = open(copy_start);

  // Restore the backup of the stack.
  for (int i = 0; i < len; i++) {
    copy_start[i] = stack_backup[i];
  }

  if (fd >= 0) {
    fail("Second open should have failed!");
  }
}
