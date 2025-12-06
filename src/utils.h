/**
 * @file utils.h / header file for program utilities.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Nov 11, 2025 / Fall 2025
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "socket.h"

#ifndef UTILS_H
#define UTILS_H

/**
 * @brief splits the provided path into filename and parent directory for the read file.
 *
 * @param path const char * - path input from the CLI args
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void split_read_path(const char *path, socket_t* sock);

/**
 * @brief splits the provided path into filename and parent directory for the write file.
 *
 * @param path const char * - path input from the CLI args
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void split_write_path(const char *path, socket_t* sock);

#endif