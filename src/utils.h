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

void split_read_path(const char *path, socket_t* sock);

void split_write_path(const char *path, socket_t* sock);

#endif