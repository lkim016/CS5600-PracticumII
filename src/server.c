/**
 * @file server.c / source file for TCP Socket Server.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Nov 11, 2025 / Fall 2025
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "config.h"
#include "socket.h"
#include "utils.h"

/**
 * @brief handles the CLI args commands for the server
 *
 * @param socket socket_t* - the pointer to the server socket metadata object
 */
void server_cmd_handles(socket_t* sock) {
  char* msg = NULL;
  switch(sock->command) {
    case STOP:
      msg = "Exiting Server...\n";
      send_msg(sock->client_sock_fd, msg);
      printf("%s", msg);
      free_socket(sock);
      exit(0);
      break;
    case WRITE:
      rcv_file(sock, sock->server_sock_fd);

      msg = "Server is processing...\n";
      send_msg(sock->client_sock_fd, msg);

      break;
    case GET:
      // send the file to server
      send_file(sock, sock->server_sock_fd);
      break;
    default:
      break;
  }
}

/**
 * @brief handles the CLI args commands
 *
 * @return int - 0 if success otherwise -1 for error 
 */
int main(void) {
  // int socket_desc, client_sock;
  socket_t* server_sck = create_socket();
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  
  // Create socket:
  server_sck->server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  
  if(server_sck->server_sock_fd < 0){
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");
  
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  
  // Bind to the set port and IP:
  if(bind(server_sck->server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("Couldn't bind to the port\n");
    return -1;
  }
  printf("Done with binding\n");
  
  // Listen for clients:
  if(listen(server_sck->server_sock_fd, 1) < 0){
    printf("Error while listening\n");
    close(server_sck->server_sock_fd);
    return -1;
  }
  
  while(1) { // loop through to have server continue listening until shut down
    
    printf("\nListening for incoming connections on port %d\n", PORT);

    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    server_sck->client_sock_fd = accept(server_sck->server_sock_fd, (struct sockaddr*)&client_addr, &client_size);
    
    if (server_sck->client_sock_fd < 0){
      printf("Can't accept\n");
      free_socket(server_sck);
      return -1;
    }
    printf("Client connected at IP: %s and port: %i\n", 
          inet_ntoa(client_addr.sin_addr), 
          ntohs(client_addr.sin_port));

    //-------------- handle client message
    char client_message[CHUNK_SIZE];
    // Clean buffers:
    memset(client_message, '\0', sizeof(client_message));

    // Receive client's message:
    // need to process the client msg into separate cmd, local filename, server filename if sending as 1 string
    if (recv(server_sck->client_sock_fd, client_message, 
            sizeof(client_message), 0) < 0){
      printf("Couldn't receive\n");
      free_socket(server_sck);
      return -1;
    }
    printf("Msg from client: %s\n", client_message);

    int token_count = 0;
    char* token = strtok(client_message, DELIMITER);
    while (token != NULL) {
      printf("%s\n", token);
      if (token_count == 0) {
        commands cmd = str_to_cmd_enum(token);
        set_sock_command(server_sck, cmd);
      } else if (token_count == 1) {
        split_read_path(token, server_sck);
        set_sock_read_filepath(server_sck);
      } else if (token_count == 2) {
        split_write_path(token, server_sck);
        set_sock_write_filepath(server_sck);
      }
      token_count++;
      token = strtok(NULL, DELIMITER);
    }

    printf("Command: %s, Send Filename: %s, Receive Filename: %s\n", cmd_enum_to_str(server_sck->command), server_sck->read_filepath, server_sck->write_filepath);
    print_write_file_info(server_sck);

    server_cmd_handles(server_sck);
    
    // Closing the socket:
    close(server_sck->client_sock_fd);
  }

  free_socket(server_sck);
    
  return 0;
}
