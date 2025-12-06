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

typedef enum { NULL_VAL = 0, WRITE, GET, RM, STOP } commands;

/**
 * @brief Represents a message.
 */
typedef struct socket_data {
    int client_sock_fd; // file descriptor for the client socket
    int server_sock_fd; // file descriptor for the server socket
    commands command; // command to be executed
    char *read_pdir; // filename of the file being sent
    char *read_filename;
    char *read_filepath;
    char *write_pdir; // filename of the file being received
    char *write_filename;
    char *write_filepath;
} socket_t;

/**
 * @brief Create and initialize a server socket object on the heap.
 *
 * @return socket_t* - return the initialized cache object
 */
socket_t* create_socket();

commands str_to_cmd_enum(const char* str);

const char *cmd_enum_to_str(commands cmd);

void set_sock_command(socket_t* sock, commands command);

void split_read_path(const char *path, socket_t* sock);

void split_write_path(const char *path, socket_t* sock);

void print_read_file_info(socket_t* sock);

void set_sock_read_filepath(socket_t* sock, const char* read_filepath);

void set_sock_write_filepath(socket_t* sock, const char* write_filepath);

void free_socket(socket_t* sock);

void send_file(socket_t* sock);

void rcv_file(socket_t* sock);

#endif