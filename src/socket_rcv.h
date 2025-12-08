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
#include <sys/socket.h>
#include <unistd.h>
#include <stddef.h>

#include "config.h"
#include "socket_md.h"

#ifndef SOCKET_RCV_H
#define SOCKET_RCV_H


ssize_t recv_all(int sock_fd, void *buffer, size_t length);

void rcv_request(socket_md_t* sock);

/**
 * @brief depending on the command, a TCP socket is being asked to recevied a read file from the other TCP socket and then write it out to its local env.
 *
 * 
 * @param sock_fd int - the socket file descriptor that the file data will be received by
 * @param sec_filepath socket_md_t* - the pointer to the socket metadata object
 * @return ssize_t -> file bytes received
 */
ssize_t rcv_file(int sock_fd, const char* sec_filepath, uint32_t size);

#endif