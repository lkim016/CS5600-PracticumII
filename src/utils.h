/**
 * @file utils.h / header file for program utilities.
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

#include <pthread.h>

#include "config.h"
#include "socket_md.h"

#ifndef UTILS_H
#define UTILS_H

/**
 * @brief constructs a message that is concatenated in dynamic memory allocation to be sent over TCP
 *
 * @param part1 const char* - a constant char string that will make up the first part of the new string
 * @param part2 const char* - a constant char string that will make up the second part of the new string
 */
char* dyn_msg(unsigned long id, const char* part1, const char* part2);

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
 * @return int - 1 for success, -1 for error, 0 for fail
 */
int rm_file_or_folder(socket_md_t* sock);

/**
 * @brief depending on the command, a TCP socket is sending a message to the other TCP socket either client or message.
 *
 * @param sock_fd int - the socket file descriptor that the file data will be sent to
 * @param message const char* - a constant char string that is the message
 * @return int - 1 for success, -1 for error, 0 for fail
 */
int send_msg(int sock_fd, const char* message);

/**
 * @brief depending on the command, a TCP socket is being asked to read a file from its local env and then send it to the other TCP socket either client or message.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 * @param sock_fd int - the socket file descriptor that the file data will be sent to
 * @return int - 1 for success, -1 for error, 0 for fail
 */
int send_file(socket_md_t* sock, int sock_fd);

/**
 * @brief depending on the command, a TCP socket is being asked to recevied a read file from the other TCP socket and then write it out to its local env.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 * @param sock_fd int - the socket file descriptor that the file data will be received by
 * @return int - 1 for success, -1 for error, 0 for fail
 */
int rcv_file(socket_md_t* sock, int sock_fd);

#endif