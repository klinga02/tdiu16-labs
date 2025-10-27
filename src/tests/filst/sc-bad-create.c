#include <stdio.h>
#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

const char filename[] = "name";

void test_main(void)
{
  // Create a file that is 32 KiB large.
  CHECK(create(filename, 32 * 1024), "Creating a 32 KiB file");
}
