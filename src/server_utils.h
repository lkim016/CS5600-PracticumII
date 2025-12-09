/**
 * @file server_utils.h / header file for Client TCP Server utilitis.
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
#include <pthread.h>
#include <stdbool.h>

#include "config.h"
#include "socket_md.h"
#include "utils.h"
#include "socket_send.h"
#include "socket_rcv.h"

#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H


extern pthread_mutex_t stop_mutex;
extern bool stop_server;


/**
 * @brief handles the CLI args commands for the server
 *
 * @param socket socket_t* - the pointer to the server socket metadata object
 */
void server_cmd_handler(socket_md_t* sock);


/**
 * @brief function to globally handle the STOP command for all threads
 *
 * @param exit_msg char* - a const char string that holds the server STOP message
 */
// void handle_stop(const char* exit_msg)

/**
 * @brief receives the message built from args received by Client
 *
 * @param socket socket_t* - the pointer to the server socket metadata object
 * @return char* - the string CLI args message
 */
char* rcv_args_message(int sock_fd);


/**
 * @brief receives the args message from the Client and distinguishes the CLI args commands into the respective member fields of the socket metadata obj
 *
 * @param socket socket_md_t* - the pointer to the client socket metadata object
 */
void set_server_sock_metadata(socket_md_t* sock, char* message);

#endif