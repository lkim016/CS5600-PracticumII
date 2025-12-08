/**
 * @file socket_send.h / header code for Socket send.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h>

#include "config.h"


#ifndef SOCKET_SEND_H
#define SOCKET_SEND_H


/**
 * @brief depending on the command, a TCP socket is sending a message to the other TCP socket either client or message.
 *
 * @param sock_fd int - the socket file descriptor that the file data will be sent to
 * @param message const char* - a constant char string that is the message
 * @return int -> 1 for success, -1 for error, 0 for fail
 */
ssize_t send_msg(int sock_fd, const char* message);


ssize_t send_file_size(int sock_fd, long size);

/**
 * @brief depending on the command, a TCP socket is being asked to read a file from its local env and then send it to the other TCP socket either client or message.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 * @param sock_fd int - the socket file descriptor that the file data will be sent to
 * @return int -> 1 for success, -1 for error, 0 for fail
 */
ssize_t send_file(const char* first_filepath, int sock_fd);


#endif