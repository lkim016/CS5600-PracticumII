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
  if (strcmp(sock->command, "STOP") == 0) {
      printf("Exiting Server...\n");
      free_socket(sock);
      exit(0);
  } else if (strcmp(sock->command, "WRITE") == 0) {

    /* // sample from original
    if (recv(client_sock, client_message, 
           sizeof(client_message), 0) < 0){
      printf("Couldn't receive\n");
      close(socket_desc);
      close(client_sock);
      return -1;
    }
    */

    rcv_file(sock->client_sock, sock->rcv_filename);
  }

}

int main(void) {
  // int socket_desc, client_sock;
  socket_t* socket_data = create_socket();
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  
  // Create socket:
  socket_data->server_sock = socket(AF_INET, SOCK_STREAM, 0);
  
  if(socket_data->server_sock < 0){
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");
  
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  
  // Bind to the set port and IP:
  if(bind(socket_data->server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("Couldn't bind to the port\n");
    return -1;
  }
  printf("Done with binding\n");
  
  // Listen for clients:
  if(listen(socket_data->server_sock, 1) < 0){
    printf("Error while listening\n");
    close(socket_data->server_sock);
    return -1;
  }
  
  while(1) { // loop through to have server continue listening until shut down
    
    printf("\nListening for incoming connections on port %d\n", PORT);

    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    socket_data->client_sock = accept(socket_data->server_sock, (struct sockaddr*)&client_addr, &client_size);
    
    if (socket_data->client_sock < 0){
      printf("Can't accept\n");
      free_socket(socket_data);
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
    if (recv(socket_data->client_sock, client_message, 
            sizeof(client_message), 0) < 0){
      printf("Couldn't receive\n");
      free_socket(socket_data);
      return -1;
    }
    printf("Msg from client: %s\n", client_message);

  //-----------
  char server_message[CHUNK_SIZE];
    
  memset(server_message, '\0', sizeof(server_message));
  // Respond to client:
  strcpy(server_message, "Server is processing sent message...");
  // printf("%s\n", server_message); // check
  if (send(socket_data->client_sock, server_message, strlen(server_message), 0) < 0){
    printf("Can't send\n");
    free_socket(socket_data);
    exit(1);
  }
  //----------
  int token_count = 0;
  char* token = strtok(client_message, DELIMITER);
  while (token != NULL) {
    printf("%s\n", token);
    if (token_count == 0) {
      set_sock_command(socket_data, token);
    } else if (token_count == 1) {
      set_sock_send_fn(socket_data, token);
    } else if (token_count == 2) {
      set_sock_rcv_fn(socket_data, token);
    }
    token_count++;
    token = strtok(NULL, DELIMITER);
  }

    // separate the message by space
    server_cmd_handles(socket_data);
    
    // Closing the socket:
    close(socket_data->client_sock);
  }

  free_socket(socket_data);
    
  return 0;
}
