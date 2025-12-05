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


void client_cmd_handles(socket_t* sock) {

    // handle different commands
    switch (sock->command) {
      case WRITE:
        if (sock->read_filename == NULL) { // need to handle the file within the command since it can differ based on the command
          printf("Invalid file path\n");
          free_socket(sock);
          exit(1);
        }

        // send the file to server
        set_sock_read_fn(sock, sock->read_filename);
        send_file(sock);
        /*
      } else if(strcmp(command, "GET") == 0) { // For GET, no file data is sent but still need to send the command, filename, and 
        break;
      } else if(strcmp(command,  "RM") == 0) {
        // For GET and RM, no file data is sent
        if (send_message(socket, client_message, strlen(client_message), NULL, 0) < 0) {
            printf("Failed to send message\n");
        }
      */
      break;
      case STOP:
        const char* cmd_str = cmd_enum_to_str(sock->command);
        if(send(sock->client_sock_fd, cmd_str, strlen(cmd_str), 0) < 0) {
          printf("Unable to send message\n");
          free_socket(sock);
          return;
        }
        break;
      default:
        printf("Unknown command\n");
        break;
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
  } else if (argc < 3 && (str_to_cmd_enum(argv[1]) == WRITE || str_to_cmd_enum(argv[1]) == GET || str_to_cmd_enum(argv[1]) == RM)) {
      printf("Usage: %s %s <CLIENT FILENAME> <SERVER FILENAME>\n", argv[0], argv[1]);
      return -1;
  } else if ((argc >= 3 && argc < 5) || str_to_cmd_enum(argv[1])== STOP) { // if local file is omitted then use current folder
    // int socket_desc;
    socket_t* client_sck = create_socket();
    struct sockaddr_in server_addr; // https://thelinuxcode.com/sockaddr-in-structure-usage-c/
    char server_message[MSG_SIZE];
    
    // Clean buffers:
    memset(server_message,'\0',sizeof(server_message));
    
    // Create socket:
    client_sck->client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(client_sck->client_sock_fd < 0){
      printf("Unable to create socket\n");
      free_socket(client_sck);
      return -1;
    }
    
    printf("Socket created successfully\n");
    
    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // Send connection request to server:
    if(connect(client_sck->client_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
      printf("Unable to connect\n");
      free_socket(client_sck);
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
    
    // set members of socket object
    set_sock_command(client_sck, str_to_cmd_enum(argv[1]));
    if (argc > 2) {
        set_sock_read_fn(client_sck, argv[2]);
    }
    if (argc > 3) {
        set_sock_write_fn(client_sck, argv[3]);
    }
    //----------- 
    // Declare client message - since its a stream will send as comma-delimited string
    char client_message[msg_size];
    // Clean buffer:
    memset(client_message,'\0',sizeof(client_message));

    // Construct the message with delimiters
    // example: ./rfs WRITE data/file.txt remote/file.txt
    // becomes: WRITE, data/file.txt, remote/file.txt
    for(int i = 1; i < argc; i++) {
      strcat(client_message, argv[i]);
      strcat(client_message, DELIMITER);
      // account for if there's an omitted file name in the client msg and the argv[]
    }

    printf("Message size: %d\n", msg_size);
    printf("Client Message: %s\n", client_message);
    
    // send client message
    if(send(client_sck->client_sock_fd, client_message, strlen(client_message), 0) < 0){
      printf("Unable to send message\n");
      free_socket(client_sck);
      return -1;
    }
    //-----------

    client_cmd_handles(client_sck);
    
    // Receive the server's response:
    if(recv(client_sck->client_sock_fd, server_message, sizeof(server_message), 0) < 0){
      printf("Error while receiving server's msg\n");
      close(client_sck->client_sock_fd);
      return -1;
    }
    
    printf("Server's response: %s\n",server_message);
    
    // Close the socket:
    free_socket(client_sck);
  }
  
  return 0;
}
