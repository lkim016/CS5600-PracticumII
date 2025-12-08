/**
 * @file utils.h / header file for file/folder utilities.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <limits.h> // for LONG_MAX

#include <pthread.h>

#include "config.h"
#include "socket_md.h"

#ifndef UTILS_H
#define UTILS_H


/**
 * @brief checks if the provided filepath is an existing file
 *
 * @param file const char* - a const string of the filepath to check
 * @return bool -> 0 for success, 1 for fail
 */
bool file_exists(const char* file);


long get_file_size(const char* filepath);

/**
 * @brief checks to see if each folder in the path exists, if not then it creates it
 *
 * @param folder_path const char* - a constant char string that is the socket metadata object's folder path (write_dirs)
 */
int folder_not_exists_make(const char* folder_path);

/**
 * @brief called by RM command to remove a file or folder indicated by first file argument
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 * @return int -> 1 for success, -1 for error, 0 for fail
 */
int rm_file_or_folder(socket_md_t* sock);


#endif