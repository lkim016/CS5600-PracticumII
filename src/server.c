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


void server_cmd_handles(int socket, int client_sock, const char *cmd, const char* server_filename) {
  if (strcmp(cmd, "STOP") == 0) {
      printf("Exiting Server...\n");
      close(socket);
      close(client_sock);
      exit(0);
  } else if (strcmp(cmd, "WRITE") == 0) {

    /* // sample from original
    if (recv(client_sock, client_message, 
           sizeof(client_message), 0) < 0){
      printf("Couldn't receive\n");
      close(socket_desc);
      close(client_sock);
      return -1;
    }
    */

    rcv_file(client_sock, server_filename);
  }

}

int main(void) {
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  
  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  
  if(socket_desc < 0){
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");
  
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  
  // Bind to the set port and IP:
  if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("Couldn't bind to the port\n");
    return -1;
  }
  printf("Done with binding\n");
  
  // Listen for clients:
  if(listen(socket_desc, 1) < 0){
    printf("Error while listening\n");
    close(socket_desc);
    return -1;
  }
  
  while(1) { // loop through to have server continue listening until shut down
    
    printf("\nListening for incoming connections on port %d\n", PORT);

    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    
    if (client_sock < 0){
      printf("Can't accept\n");
      close(socket_desc);
      close(client_sock);
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
    if (recv(client_sock, client_message, 
            sizeof(client_message), 0) < 0){
      printf("Couldn't receive\n");
      close(client_sock);
      return -1;
    }
    printf("Msg from client: %s\n", client_message);

  //-----------
  char server_message[CHUNK_SIZE];
    
  memset(server_message, '\0', sizeof(server_message));
  // Respond to client:
  strcpy(server_message, "Server is processing sent message...");
  // printf("%s\n", server_message); // check
  if (send(client_sock, server_message, strlen(server_message), 0) < 0){
    printf("Can't send\n");
    close(socket_desc);
    close(client_sock);
    exit(1);
  }
  //----------

    // Get client message and break it down by delimiter - get command size, local filenam size, server filename size
  int cmd_len = 0;
  int local_fname_len = 0; // maybe can put this size in an array or receive it at the time of the send?
  int server_fname_len = 0;
  int token_count = 0;
  char* token = strtok(client_message, DELIMITER);
  while (token != NULL) {
    printf("%s\n", token);
    if (cmd_len == 0) {
        cmd_len = strlen(token);
    } else if (local_fname_len == 0) {
        local_fname_len = strlen(token);
    } else if (server_fname_len == 0) {
        server_fname_len = strlen(token);
    }
    token_count++;
    token = strtok(NULL, DELIMITER);
  }

  // Store command
  int src_str_index = 0;
  char command[cmd_len + 1];
  strncpy(command, client_message + src_str_index, cmd_len);
  command[cmd_len] = '\0'; // null terminate the string
  // store local filename
  src_str_index = cmd_len + 1;
  char local_filename[local_fname_len + 1];
  strncpy(local_filename, client_message + src_str_index, local_fname_len);
  local_filename[local_fname_len] = '\0';
  // store server filename
  src_str_index += local_fname_len + 1;
  char server_filename[server_fname_len + 1];
  strncpy(server_filename, client_message + src_str_index, server_fname_len);
  server_filename[server_fname_len] = '\0';

    // separate the message by space
    server_cmd_handles(socket_desc, client_sock, command, server_filename);
    
    // Closing the socket:
    close(client_sock);
  }

  close(socket_desc);
    
  return 0;
}
