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

#include "config.h"
#include "socket_md.h"
#include "utils.h"
#include "socket_send.h"
#include "socket_rcv.h"

#ifndef HANDLERS_H
#define HANDLERS_H


void push(int sock_fd, char* filepath1, uint32_t file_size);

void pull(int sock_fd, char* filepath2, uint32_t file_size);

#endif