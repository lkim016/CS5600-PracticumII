/**
 * @file client_test.c / source code for Client socket tests.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "../src/client_utils.h"
#include "test_utils.h"


int main(void) {

    // TEST: edge case - 2 args with WRITE command
    char* msg = "./rfs WRITE";
    int status = system(msg);
    print_check(status == 1, msg);

    // TEST: edge case - 5 args with WRITE command
    msg = "./rfs WRITE data.txt hello.txt STOP";
    status = system(msg);
    print_check(status == 1, msg);

    // // TEST: happy case - 3 args - result will display whole interaction with the server depending on the case
    // msg = "./rfs WRITE data.txt hello.txt STOP";
    // status = system(msg);
    // print_check(status == 1, msg);

    // // TEST: happy case - 4 args
    // msg = "./rfs WRITE data.txt hello.txt STOP";
    // status = system(msg);
    // print_check(status == 1, msg);


    // TEST: test STOP command 
    msg = "./rfs STOP";
    status = system(msg);
    print_check(status == 0, msg);

    // TEST: test construct_message
    msg = "check client build_message";
    char* test[] = {"./rfs", "GET", "flower.jpg", "forest5.jpg"};
    int count = sizeof(test)/sizeof(test[0]);
    char* result = build_message(count, test);
    char* exp = "GET,flower.jpg,forest5.jpg,";
    print_check(strcmp(result, exp) == 0, msg);

    return 0;
}

