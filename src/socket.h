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

/**
 * @brief Represents a message.
 */
typedef struct socket_data {
    int client_sock; // file descriptor for the client socket
    int server_sock; // file descriptor for the server socket
    char *command; // command to be executed
    char* send_filename; // filename of the file being sent
    char* rcv_filename; // filename of the file being received
} socket_t;

/**
 * @brief Create and initialize a server socket object on the heap.
 *
 * @return socket_t* - return the initialized cache object
 */
socket_t* create_socket();

void set_sock_command(socket_t* sock, const char* command);

void set_sock_send_fn(socket_t* sock, const char* send_filename);

void set_sock_rcv_fn(socket_t* sock, const char* rcv_filename);

void free_socket(socket_t* sock);

void send_file(int send_socket, const char* read_filename);

void rcv_file(int rcv_socket, const char* write_filename);

#endif