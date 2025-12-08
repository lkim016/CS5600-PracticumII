/**
 * @file test_utils.c / source code for test utilites.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "test_utils.h"

/*
print_check
*/
void print_check(bool status, char* msg) {
    if (status) {
        printf("SUCCESS: %s\n", msg);
    } else {
        printf("FAIL: %s\n", msg);
    }
}