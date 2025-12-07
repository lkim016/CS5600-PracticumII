/**
 * @file utils_test.c / source code for program utilities tests.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
// #include "../src/client.c"


/**
 * @brief handles a failed system call
 *
 * @param message char* - custom message to display in terminal
 * @return -1 to signal failed execution of the program
 */
int failed_command(char *message) {
    printf("ERROR: %s\n", message);
    return -1;
}

int main(void) {

    // TEST: test the command args in client - 2, 3
    char* msg = "./rfs WRITE";
    int status = system(msg);
    if (status != 0) {
        failed_command(msg);
    }

    // TEST: edge case - 5

    // TEST: test the command args in client - 2, 3
    msg = "./rfs STOP";
    status = system(msg);
    if (status != 0) {
        return failed_command(msg);
    }

    return 0;
}

