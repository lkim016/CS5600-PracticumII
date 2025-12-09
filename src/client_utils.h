/**
 * @file client_utils.h / header file for Client TCP Socket utilitis.
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

#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

/**
 * @brief handles the CLI args commands for the client
 * WRITE - get read file size and send to server
 * GET 
 * RM
 * STOP
 *
 * @param socket socket_md_t* - the pointer to the client socket metadata object
 * @return int -> 1 for success, -1 for error, 0 for fail
 */
void client_cmd_handler(socket_md_t* sock);

/**
 * @brief distinguishes the CLI args commands into the respective member fields of the socket metadata obj
 *
 * @param socket socket_md_t* - the pointer to the client socket metadata object
 * @param argc int - count of arguments input from the CLI
 * @param argv char* - arguments input from the CLI
 */
void set_client_sock_metadata(socket_md_t* sock, int argc, char* argv[]);


/**
 * @brief sends constructed message from the CLI args commands to be sent to the Server
 *
 * @param response char* - the response sent by the server
 */
void __print_server_resp(const char* response);

#endif