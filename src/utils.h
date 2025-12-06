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
#include <sys/stat.h>

#include "config.h"
#include "socket.h"

#ifndef UTILS_H
#define UTILS_H

int folder_not_exists_make(const char* folder_path);

/**
 * @brief depending on the command, a TCP socket is sending a message to the other TCP socket either client or message.
 *
 * @param sock_fd int - the socket file descriptor that the file data will be sent to
 * @param message const char* - a constant char string that is the message
 */
void send_msg(int sock_fd, const char* message);

/**
 * @brief depending on the command, a TCP socket is being asked to read a file from its local env and then send it to the other TCP socket either client or message.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 * @param sock_fd int - the socket file descriptor that the file data will be sent to
 */
void send_file(socket_t* sock, int sock_fd);

/**
 * @brief depending on the command, a TCP socket is being asked to recevied a read file from the other TCP socket and then write it out to its local env.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 * @param sock_fd int - the socket file descriptor that the file data will be received by
 */
void rcv_file(socket_t* sock, int sock_fd);

#endif