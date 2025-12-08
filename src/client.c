/**
 * @file client.c / source file for TCP Socket Client.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "client_utils.h"



/**
 * @brief initiates a TCP socket client network connection to specified server
 * 
 * @param argc int - count of arguments input from the CLI
 * @param argv char* - arguments input from the CLI
 * @return int - 0 if success otherwise -1 for error 
 */
int main(int argc, char* argv[]) {

  if (((argc > 2 && argc < 5) && (strcmp(argv[1], "WRITE") == 0 || strcmp(argv[1], "GET") == 0 || strcmp(argv[1], "RM") == 0)) || strcmp(argv[1], "STOP") == 0) {
  
    int socket_desc;
    struct sockaddr_in server_addr; // https://thelinuxcode.com/sockaddr-in-structure-usage-c/
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
      printf("Unable to create socket\n");
      close(socket_desc);
      return -1;
    }
    
    printf("Socket created successfully-----\n");
    
    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // Send connection request to server:
    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
      printf("Unable to connect\n");
      close(socket_desc);
      return -1;
    }
    printf("Connected with server successfully\n");

    socket_md_t* client_metadata = create_socket_md(socket_desc);
    if (!client_metadata) {
        printf("Failed to create socket metadata\n");
        return -1;
    }

    // char* msg_to_server = build_message(argc, argv);
    // ssize_t cmd_sent_status = send_args_message(client_metadata, msg_to_server);
    
    // Set server socket metadata
    set_client_sock_metadata(client_metadata, argc, argv);

    print_sock_metada(client_metadata); // FIXME: maybe delete
    
    client_cmd_handler(client_metadata);

    
    // Close the socket:
    free_socket(client_metadata);
    printf("CLIENT END---\n");
  } else {
    printf("Usage: %s <COMMAND> <CLIENT FILENAME> <SERVER FILENAME>\n", argv[0]);
    return 1;
  }
  
  
  return 0;
}
