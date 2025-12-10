/**
 * @file handlers.h / header file for request handlers.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "config.h"
#include "socket_md.h"
#include "utils.h"
#include "socket_send.h"
#include "socket_rcv.h"

#ifndef HANDLERS_H
#define HANDLERS_H

char* deliver(unsigned long t_id, int sock_fd, char* filepath1, uint32_t file_size);

char* receive(unsigned long t_id, int sock_fd, char* filepath2, uint32_t file_size);

/**
 * @brief constructs a message that is concatenated in dynamic memory allocation to be sent over TCP
 *
 * @param part1 const char* - a constant char string that will make up the first part of the new string
 * @param part2 const char* - a constant char string that will make up the second part of the new string
 */
char* build_send_msg(unsigned long id, const char* part1, const char* part2); // RM uses this with 3 args


#endif