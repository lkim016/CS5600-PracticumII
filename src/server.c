/**
 * @file server.c / source file for TCP Socket Server.
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

#include "config.h"
#include "socket.h"
#include "utils.h"

/**
 * @brief handles the CLI args commands for the server
 *
 * @param socket socket_t* - the pointer to the server socket metadata object
 */
void server_cmd_handles(socket_t* sock) {
  if (!sock) {
      fprintf(stderr, "ERROR: Socket is NULL\n");
      return;
  }

  char* msg = NULL;
  switch(sock->command) {
    case WRITE:
      if (folder_not_exists_make(sock->write_dirs) == 1) {
          if (rcv_file(sock, sock->server_sock_fd) < 0 ) {
            msg = "Error receiving file\n";
          } else {
            msg = "Server is processing...\n";
          }
        } else {
          msg = "Warning: File was not received - issues with folder path to write out to\n";
        }

        if (send_msg(sock->client_sock_fd, msg) < 0) {
            perror("Failed to send response to client");
        }

      break;
    case GET:
      // send the file to server
      send_file(sock, sock->server_sock_fd);
      break;
    case STOP:
      msg = "Exiting Server...\n";
      send_msg(sock->client_sock_fd, msg);
      printf("%s", msg);
      free_socket(sock);
      exit(0);
      break;
    default:
      printf("Unknown command\n");
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
  if (!server_sck) {
      printf("Failed to set socket metadata\n");
      return -1;
  }
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  
  // Create socket:
  server_sck->server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  
  if(server_sck->server_sock_fd < 0){
    printf("Error while creating socket\n");
    free_socket(server_sck);
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
    free_socket(server_sck);
    return -1;
  }
  printf("Done with binding\n");
  
  // Listen for clients:
  if(listen(server_sck->server_sock_fd, 1) < 0){
    printf("Error while listening\n");
    free_socket(server_sck);
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
      printf("Failed to receive message from client\n");
      close(server_sck->client_sock_fd);
      continue;
    }
    printf("Msg from client: %s\n", client_message);

    const char* first_path = NULL;
    int token_count = 0;
    char* token = strtok(client_message, DELIMITER);
    while (token != NULL) {
      printf("%s\n", token);
      if (token_count == 0) {
        commands cmd = str_to_cmd_enum(token);
        set_sock_command(server_sck, cmd);
      } else if (token_count == 1) {
        set_read_fileInfo(token, server_sck);
        set_read_file_ext(server_sck);
        set_sock_read_filepath(server_sck);
        first_path = token;
      } else if (token_count == 2) {
        set_write_fileInfo(token, server_sck);
        set_write_file_ext(server_sck);
        set_sock_write_filepath(server_sck);
      }
      token_count++;
      token = strtok(NULL, DELIMITER);
    }

    if (server_sck->write_filename == NULL) { // if 3 command is omitted
      set_write_fileInfo(first_path, server_sck);
      set_write_file_ext(server_sck);
      set_sock_write_filepath(server_sck);
    }

    printf("Command: %s, Send Filename: %s, Receive Filename: %s\n", cmd_enum_to_str(server_sck->command), server_sck->read_filepath, server_sck->write_filepath);
      print_write_file_info(server_sck); // FIXME: maybe delete

    server_cmd_handles(server_sck);
    
    // Closing the socket:
    close(server_sck->client_sock_fd);
  }

  // Clean up server socket
  free_socket(server_sck);
    
  return 0;
}
