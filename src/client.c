/*
 * client.c -- TCP Socket Client
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


void client_cmd_handles(int socket, char* args[]) {
    // const char *command, const char *file_path
    const char *command = args[1];

    // handle different commands
    if(strcmp(command, "WRITE") == 0) {
      if (args[2] == NULL) { // need to handle the file within the command since it can differ based on the command
        printf("Invalid file path\n");
        exit(1);
      }

      // send the file to server
      const char *local_fn = args[2];
      send_file(socket, local_fn);
      /*
    } else if(strcmp(command, "GET") == 0) { // For GET, no file data is sent but still need to send the command, filename, and 
      break;
    } else if(strcmp(command,  "RM") == 0) {
      // For GET and RM, no file data is sent
      if (send_message(socket, client_message, strlen(client_message), NULL, 0) < 0) {
          printf("Failed to send message\n");
      }
    */
    } else if (strcmp(command, "STOP") == 0) {
      if(send(socket, command, strlen(command), 0) < 0) {
        printf("Unable to send message\n");
        close(socket);
        return;
      }
    } else {
      printf("Unknown command\n");
      return;
    }
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return * int 
 */
int main(int argc, char* argv[]) {

  if (argc < 2) {
      printf("Usage: %s <COMMAND> <CLIENT FILENAME> <SERVER FILENAME>\n", argv[0]);
      return -1;
  } else if (argc < 3 && (strcmp(argv[1], "WRITE") == 0 || strcmp(argv[1], "GET") == 0 || strcmp(argv[1], "RM") == 0)) {
      printf("Usage: %s %s <CLIENT FILENAME> <SERVER FILENAME>\n", argv[0], argv[1]);
      return -1;
  } else if ((argc >= 3 && argc < 5) || strcmp(argv[1], "STOP") == 0) { // if local file is omitted then use current folder
    int socket_desc;
    struct sockaddr_in server_addr; // https://thelinuxcode.com/sockaddr-in-structure-usage-c/
    char server_message[MSG_SIZE];
    
    // Clean buffers:
    memset(server_message,'\0',sizeof(server_message));
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
      printf("Unable to create socket\n");
      close(socket_desc);
      return -1;
    }
    
    printf("Socket created successfully\n");
    
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
    
    //-------------- construct client message
    int msg_size = 0;
    // Calculate message length when combined by delimiters - command,filename,file_size,remote_filename
    for(int i = 1; i < argc; i++) {
      msg_size += strlen(argv[i]);
      msg_size += 1; // for comma or null terminator
    }

    if (msg_size < 1) {
        printf("Invalid message size\n");
        exit(1);
    }
    // account for null file name
    // Declare client message
    char client_message[msg_size];
    // Clean buffer:
    memset(client_message,'\0',sizeof(client_message));

    // Construct the message with delimiters
    // example: ./rfs WRITE data/file.txt remote/file.txt
    // becomes: WRITE, data/file.txt, remote/file.txt
    for(int i = 1; i < argc; i++) {
      strcat(client_message, argv[i]);
      if (i < argc - 1) {
          strcat(client_message, DELIMITER);
      }
      // account for if there's an omitted file name in the client msg and the argv[]
    }

    printf("Message size: %d\n", msg_size);
    printf("Client Message: %s\n", client_message);
    
    // send client message
    if(send(socket_desc, client_message, strlen(client_message), 0) < 0){
      printf("Unable to send message\n");
      close(socket_desc);
      return -1;
    }
    //--------------

    client_cmd_handles(socket_desc, argv);
    
    // Receive the server's response:
    if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
      printf("Error while receiving server's msg\n");
      close(socket_desc);
      return -1;
    }
    
    printf("Server's response: %s\n",server_message);
    
    // Close the socket:
    close(socket_desc);
  }
  
  return 0;
}
