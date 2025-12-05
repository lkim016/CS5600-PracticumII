/*
 * server.c -- TCP Socket Server
 * 
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


void server_cmd_handles(socket_t* sock) {
  switch(sock->command) {
    case STOP:
      printf("Exiting Server...\n");
      free_socket(sock);
      exit(0);
      break;
    case WRITE:
      rcv_file(sock);
      break;
    default:
      break;
  }

}

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

  //-----------
  char server_message[CHUNK_SIZE];
    
  memset(server_message, '\0', sizeof(server_message));
  // Respond to client:
  strcpy(server_message, "Server is processing sent message...");
  // printf("%s\n", server_message); // check
  if (send(server_sck->client_sock_fd, server_message, strlen(server_message), 0) < 0){
    printf("Can't send\n");
    free_socket(server_sck);
    exit(1);
  }
  //----------
  int token_count = 0;
  char* token = strtok(client_message, DELIMITER);
  while (token != NULL) {
    printf("%s\n", token);
    if (token_count == 0) {
      commands cmd = str_to_cmd_enum(token);
      set_sock_command(server_sck, cmd);
    } else if (token_count == 1) {
      set_sock_read_fn(server_sck, token);
    } else if (token_count == 2) {
      set_sock_write_fn(server_sck, token);
    }
    token_count++;
    token = strtok(NULL, DELIMITER);
  }

  printf("Command: %s, Send Filename: %s, Receive Filename: %s\n", cmd_enum_to_str(server_sck->command), server_sck->read_filename, server_sck->write_filename);

    // separate the message by space
    server_cmd_handles(server_sck);
    
    // Closing the socket:
    close(server_sck->client_sock_fd);
  }

  free_socket(server_sck);
    
  return 0;
}
