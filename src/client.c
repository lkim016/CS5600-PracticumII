/**
 * @file client.c / source file for TCP Socket Client.
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
 * @brief handles the CLI args commands for the client
 *
 * @param socket socket_t* - the pointer to the client socket metadata object
 */
void client_cmd_handles(socket_t* sock) {

  // handle different commands
  char* msg = NULL;
  switch (sock->command) {
    case STOP:
      // Receive the server's response:
      if(recv(sock->client_sock_fd, msg, sizeof(msg), 0) < 0) {
        printf("Error while receiving server's msg\n");
        free_socket(sock);
        return exit(1);
      }
      
      printf("Server's response: %s\n", msg);
      exit(0);
      break;
    case WRITE:
      // send the file to server
      send_file(sock, sock->client_sock_fd);
      break;
    case GET:
      rcv_file(sock, sock->client_sock_fd);
      
      msg = "Client is processing...\n";
      send_msg(sock->server_sock_fd, msg);

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
    default:
      printf("Unknown command\n");
      break;
    }
}

/**
 * @brief initiates a TCP socket client network connection
 * 
 * @param argc int - count of arguments input from the CLI
 * @param argv char* - arguments input from the CLI
 * @return int - 0 if success otherwise -1 for error 
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
        return -1;
    }
    
    // set members of socket object
    set_sock_command(client_sck, str_to_cmd_enum(argv[1]));
    if (argc > 2) { // set read_filepath
        split_read_path(argv[2], client_sck);
        set_sock_read_filepath(client_sck);
    }
    if (argc > 3) { // WRITE - if argv[3] is null then use file name of arfv[2] / GET - if argv[3] is null then need to use default local path
        split_write_path(argv[3], client_sck);
        set_sock_write_filepath(client_sck);
    }

    print_read_file_info(client_sck);
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
    
    // send client message - this sends the commands
    send_msg(client_sck->client_sock_fd, client_message);
    //-----------

    client_cmd_handles(client_sck);
    
    // Close the socket:
    free_socket(client_sck);
  }
  
  return 0;
}
