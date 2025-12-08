/**
 * @file test_utils.h / header code for test utilites.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

/**
 * @brief handles a failed system call
 *
 * @param message char* - custom message to display in terminal
 * @return -1 to signal failed execution of the program
 */
void print_check(bool status, char* msg);