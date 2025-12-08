/**
 * @file socket_rcv.h / header code for Socket receive.
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
#include <pthread.h>

#include "config.h"

#ifndef SOCKET_RCV_H
#define SOCKET_RCV_H


uint32_t rcv_file_size(int sock_fd);

/**
 * @brief depending on the command, a TCP socket is being asked to recevied a read file from the other TCP socket and then write it out to its local env.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 * @param sock_fd int - the socket file descriptor that the file data will be received by
 * @return int -> 1 for success, -1 for error, 0 for fail
 */
ssize_t rcv_file(const char* sec_filepath, int sock_fd, uint32_t size);

#endif