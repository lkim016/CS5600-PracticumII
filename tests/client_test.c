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
void print_check(bool status, char* msg) {
    if (status) {
        printf("SUCCESS: %s\n", msg);
    } else {
        printf("FAIL: %s\n", msg);
    }
}

int main(void) {

    // TEST: test the command args in client - 2, 3
    char* msg = "./rfs WRITE";
    int status = system(msg);
    print_check(status == 1, msg);

    // TEST: edge case - 5
    msg = "./rfs WRITE data.txt hello.txt STOP";
    status = system(msg);
    print_check(status == 1, msg);

    // TEST: test the command args in client - 2, 3
    msg = "./rfs STOP";
    status = system(msg);
    print_check(status == 0, msg);

    return 0;
}

