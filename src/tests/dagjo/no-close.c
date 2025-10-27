/* Simple program that opens a file an purposfully crashes before
 * calling SYS_EXIT
 */

#include "tests/lib.h"
#include "files.h"
#include "tests/lib.h"

#include <syscall.h>

const char *test_name = "no-close";

int main(void)
{
    int fd = open(FILE_NAME);
    if (fd == -1) {
        fail("open %s", FILE_NAME);
    }

    msg("Congratulations - you have successfully dereferenced NULL: %d",
            *(volatile int *) NULL);
    fail("should have exited with -1");
}
