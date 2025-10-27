/*
 * Checks if child processes close open files properly.
 *
 * This file starts a child process that creates a file, opens it, and then crashes.
 *
 * If everything is correct, this program can then delete the file and create a new one.
 *
 * If the child process do not close its file, the remove will succeed but the old
 * file will remain in the file system, which causes the second create to fail
 * since the file system disk is full.
 */

#include "tests/lib.h"
#include "tests/main.h"
#include "sleep.h"
#include "files.h"

#include <stdio.h>
#include <syscall.h>

void test_main(void)
{
    // Create the file FILE_NAME in the file system.
    bool created = create(FILE_NAME, FILE_SIZE);
    if (!created) {
        fail("create %s with size %d", FILE_NAME, FILE_SIZE);
    }

    // Start the child process. It will open the newly created file and crash.
    int child = exec("no-close");
    if (child < 0) {
        fail("exec no-close");
    }
    

    // Wait for it to terminate.
    int child_exit = wait(child);
    if (child_exit != -1) {
        fail("child exited with %d, expected -1", child_exit);
    }

    // Give the code in process_cleanup some time to finish.
    sleep(1000);

    // Remove the file. This should always succeed. If the child process still has the file
    // open, its directory entry will disappear but it will not be deallocated from the file system.
    bool removed = remove(FILE_NAME);
    if (!removed) {
        fail("remove %s", FILE_NAME);
    }

    // Create the file again. Since the file is large enough to fill more than 50% of the disk,
    // this will fail if the OS did not close the file for our child process that crashed.
    created = create(FILE_NAME, FILE_SIZE);
    if (!created) {
        fail("create %s with size %d", FILE_NAME, FILE_SIZE);
    }
}
