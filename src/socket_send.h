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
#include "protocol.h"
#include "socket_md.h"


#ifndef SOCKET_SEND_H
#define SOCKET_SEND_H


/**
 * @brief constructs a message that is concatenated in dynamic memory allocation to be sent over TCP
 *
 * @param part1 const char* - a constant char string that will make up the first part of the new string
 * @param part2 const char* - a constant char string that will make up the second part of the new string
 */
char* build_send_msg(unsigned long id, const char* part1, const char* part2);


/**
 * @brief depending on the command, a TCP socket is sending a message to the other TCP socket either client or message.
 *
 * @param sock_fd int - the socket file descriptor that the file data will be sent to
 * @param message const char* - a constant char string that is the message
 * @return int -> 1 for success, -1 for error, 0 for fail
 */
ssize_t send_msg(int sock_fd, const char* message);

ssize_t send_size(int sock_fd, uint32_t size);


int send_request(socket_md_t* sock);

/**
 * @brief depending on the command, a TCP socket is being asked to read a file from its local env and then send it to the other TCP socket either client or message.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 * @param sock_fd int - the socket file descriptor that the file data will be sent to
 * @return int -> 1 for success, -1 for error, 0 for fail
 */
ssize_t send_file(const char* first_filepath, int sock_fd);


#endif