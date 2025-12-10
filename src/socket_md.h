/**
 * @file socket_md.h / header file for socket.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "config.h"
#include "protocol.h"

#ifndef SOCKET_MD_H
#define SOCKET_MD_H


typedef enum {WRITE, GET, RM, STOP, NULL_VAL = 0} commands;
typedef enum {CLIENT, SERVER} socket_type;

/**
 * @brief Represents a socket metadata.
 */
typedef struct socket_metadata {
    int client_sock_fd; // file descriptor for the client socket
    socket_type type;
    uint32_t file_size; // file size of file to be sent
    commands command; // execution commands from the program CLI args
    char *first_dirs; // parent dir of the file being read
    char *first_filename; // filename of the file being read
    char *first_file_ext; // file extension of the file being read
    char *first_filepath; // filepath of the file being read
    char *sec_dirs; // parent dir of the file being written
    char *sec_filename; // filename of the file being written
    char *sec_file_ext; // file extension of the file being written
    char *sec_filepath; // filepath of the file being written

} socket_md_t;

/**
 * @brief create and initialize a socket metadata object on the heap.
 *
 * @param client_fd int - the client socket file descriptor
 * @param type sockety_type - the client socket type
 * @return socket_md_t* - return the initialized socket metadata object
 */
socket_md_t* create_socket_md(int client_fd, socket_type type);


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
 * @param cmd commands - the command enum type
 * @return const char * - returns the string conversion of the enum command
 */
const char* cmd_enum_to_str(commands cmd);

/**
 * @brief converts an enum type command to a string type.
 *
 * @param type sock_type - the metadata type enum type
 * @return const char * - returns the string conversion of the enum command
 */
const char* type_enum_to_str(socket_type type);

/**
 * @brief sets the socket metadata object command member.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 * @param command commands - the enum type command
 */
void set_command(socket_md_t* sock, commands command);

/**
 * @brief prints out the socket metadata object read file info.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 */
void print_sock_metada(socket_md_t* sock);

/**
 * @brief sets the socket metadata object read file's filepath member.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 */
void set_first_filepath(socket_md_t* sock);

/**
 * @brief sets the socket metadata object write file's filepath member.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 */
void set_sec_filepath(socket_md_t* sock);

/**
 * @brief splits and sets the provided path into filename and folder directories for the read file info members of the socket metadata object.
 *
 * @param path const char * - path input from the CLI args
 * @param socket socket_md_t* - the pointer to the socket metadata object
 */
void set_first_fileInfo(const char *path, socket_md_t* sock);

/**
 * @brief splits and sets the provided path into filename and folder directories for the write file info members of the socket metadata object.
 *
 * @param path const char * - path input from the CLI args
 * @param socket socket_md_t* - the pointer to the socket metadata object
 */
void set_sec_fileInfo(const char *path, socket_md_t* sock);

/**
 * @brief free the socket metadata object and its members.
 *
 * @param socket socket_md_t* - the pointer to the socket metadata object
 */
void free_socket(socket_md_t* sock);

#endif