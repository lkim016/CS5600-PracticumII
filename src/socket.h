/**
 * @file socket.h / header file for socket.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Nov 11, 2025 / Fall 2025
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    char *read_dirs; // parent dir of the file being read
    char *read_filename; // filename of the file being read
    char *read_filepath; // filepath of the file being read
    char *read_file_ext; // file extension of the file being read
    char *write_dirs; // parent dir of the file being written
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
 * @brief sets the file extension from the read filename.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void set_read_file_ext(socket_t* sock);

/**
 * @brief sets the file extension from the write filename.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void set_write_file_ext(socket_t* sock);

/**
 * @brief splits the provided path into filename and parent directory for the read file.
 *
 * @param path const char * - path input from the CLI args
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void set_write_fileInfo(const char *path, socket_t* sock);

/**
 * @brief splits the provided path into filename and parent directory for the write file.
 *
 * @param path const char * - path input from the CLI args
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void set_write_fileInfo(const char *path, socket_t* sock);

/**
 * @brief free the socket metadata object and its members.
 *
 * @param socket socket_t* - the pointer to the socket metadata object
 */
void free_socket(socket_t* sock);

#endif