/**
 * @file socket.h / header file for socket.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Nov 11, 2025 / Fall 2025
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#include "config.h"

#ifndef SOCKET_H
#define SOCKET_H

void send_file(int send_socket, const char* read_filename);

void rcv_file(int rcv_socket, const char* write_filename);

#endif