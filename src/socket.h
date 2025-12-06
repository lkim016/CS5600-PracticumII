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
#include <dirent.h>
#include <string.h>

#include "config.h"

#ifndef SOCKET_H
#define SOCKET_H

typedef enum { NULL_VAL = 0, WRITE, GET, RM, STOP } commands;

/**
 * @brief Represents a socket metadata.
 */
typedef struct socket_data {
    int client_sock_fd; // file descriptor for the client socket
    int server_sock_fd; // file descriptor for the server socket
    commands command; // execution commands from the program CLI args
    char *read_pdir; // parent dir of the file being read
    char *read_filename; // filename of the file being read
    char *read_filepath; // filepath of the file being read
    char *read_file_ext; // file extension of the file being read
    char *write_pdir; // parent dir of the file being written
    char *write_filename; // filename of the file being written
    char *write_filepath; // filepath of the file being written
    char *write_file_ext; // file extension of the file being written
} socket_t;

/**
 * @brief create and initialize a socket metadata object on the heap.
 *
 * @return socket_t* - return the initialized socket metadata object
 */
socket_t* create_socket();

/**
 * @brief converts a string type command to an enum type.
 *
 * @param str const char * - the string type command
 * @return commands - returns the enum type command
 */
commands str_to_cmd_enum(const char* str);

/**
 * @brief converts an enum type command to a string type.
 *
 * @param cmd commands - the enum type command
 * @return const char * - returns the string conversion of the enum command
 */
const char* cmd_enum_to_str(commands cmd);

/**
 * @brief initializes the socket metadata object command member.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 * @param command commands - the enum type command
 */
void set_sock_command(socket_t* sock, commands command);

/**
 * @brief prints out the socket metadata object read file info.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void print_read_file_info(socket_t* sock);

/**
 * @brief prints out the socket metadata object write file info.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void print_write_file_info(socket_t* sock);

/**
 * @brief initializes the socket metadata object read file's filepath member.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void set_sock_read_filepath(socket_t* sock);

/**
 * @brief initializes the socket metadata object write file's filepath member.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void set_sock_write_filepath(socket_t* sock);

/**
 * @brief free the socket metadata object and its members.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void free_socket(socket_t* sock);

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