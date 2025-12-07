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

#include "config.h"
#include "socket_md.h"
#include "utils.h"



/**
 * @brief handles the CLI args commands for the client
 *
 * @param socket socket_md_t* - the pointer to the client socket metadata object
 */
void client_cmd_handler(socket_md_t* sock) {
  if (!sock) {
      fprintf(stderr, "ERROR: Socket is NULL\n");
      return;
  }
  
  // Clean buffers:
  char server_message[MSG_SIZE]; // Declare server message - since its a stream will send as comma-delimited string
  memset(server_message,'\0',sizeof(server_message));
  const char* msg = NULL;
  // handle different commands
  switch (sock->command) {
    case WRITE:
      // send the file to server
      // 
      send_file(sock, sock->client_sock_fd);
      // Wait for acknowledgment from the other socket before declaring success
      if (recv(sock->client_sock_fd, server_message, sizeof(server_message), 0) < 0) {
          perror("Error receiving acknowledgment from server");
          return;
      }

      printf("Server's response: %s\n",server_message);
      break;
    case GET:

      if (folder_not_exists_make(sock->sec_filepath) == 1) {
          if (rcv_file(sock, sock->client_sock_fd) < 0 ) {
            msg = "Error receiving file\n";
          } else {
            msg = "File sent successfully!\n";
          }
        } else {
          msg = "Warning: File was not received - issues with folder path to write out to\n";
        }

        if (send_msg(sock->client_sock_fd, msg) < 0) {
            perror("Failed to send response to server");
            return;
        }
    case RM:
        // Wait for acknowledgment from the other socket before declaring success
        if (recv(sock->client_sock_fd, server_message, sizeof(server_message), 0) < 0) {
            perror("Error receiving acknowledgment from server");
            return;
        }

        printf("Server's response: %s\n",server_message);

      break;
    case STOP:
      // Receive the server's response:
      if(recv(sock->client_sock_fd, server_message, sizeof(server_message), 0) < 0) {
        printf("Error while receiving server's msg\n");
        return;
      }
      
      printf("Server's response: %s\n",server_message);
      break;
    default:
      printf("Unknown command\n");
      break;
    }
}

/**
 * @brief distinguishes the CLI args commands into the respective member fields of the socket metadata obj
 *
 * @param socket socket_md_t* - the pointer to the client socket metadata object
 * @param argc int - count of arguments input from the CLI
 * @param argv char* - arguments input from the CLI
 */
void set_client_sock_metadata(socket_md_t* sock, int argc, char* argv[]) {
  // set members of socket object
  set_command(sock, str_to_cmd_enum(argv[1]));
  printf("is this the issue?");

  if (argc > 2) { // set first_filepath
      set_first_fileInfo(argv[2], sock);
      set_first_filepath(sock);
  }
  
  if (argc > 3) { // WRITE - if argv[3] is null then use file name of arfv[2] / GET - if argv[3] is null then need to use default local path
      set_sec_fileInfo(argv[3], sock);
      set_sec_filepath(sock);
  }
}

/**
 * @brief constructs message from the CLI args commands to be sent to the Server and sends
 *
 * @param argc int - count of arguments input from the CLI
 * @param argv char* - arguments input from the CLI
 */
void send_args_message(socket_md_t* sock, int argc, char* argv[]) {
    //-------------- construct client message
    int msize = 0;
    // Calculate message length when combined by delimiters - command,filename,file_size,remote_filename
    for(int i = 1; i < argc; i++) {
      msize += strlen(argv[i]) + 1; // Adding 1 for the delimiter/comma
    }

    if (msize < 1) {
        printf("Invalid message size\n");
        return;
    }
    printf("Message size: %d\n", msize);


    char server_message[msize]; // Declare server message - since its a stream will send as comma-delimited string
    // Clean buffer:
    memset(server_message,'\0',sizeof(server_message));

    // Construct the message with delimiters
    // example: ./rfs WRITE data/file.txt remote/file.txt
    // becomes: WRITE, data/file.txt, remote/file.txt
    for(int i = 1; i < argc; i++) {
      strcat(server_message, argv[i]);
      strcat(server_message, DELIMITER);
      // account for if there's an omitted file name in the client msg and the argv[]
    }

    printf("Client Message: %s\n", server_message);
    
    // send client message - this sends the commands
    int sent_size = send_msg(sock->client_sock_fd, server_message);
    if (sent_size > 0) {
      printf("Message of size %d successfully sent\n", sent_size);
    }
    // printf("Sent size: %d\n", sent_size);
}

/**
 * @brief initiates a TCP socket client network connection to specified server
 * 
 * @param argc int - count of arguments input from the CLI
 * @param argv char* - arguments input from the CLI
 * @return int - 0 if success otherwise -1 for error 
 */
int main(int argc, char* argv[]) {

  if (argc < 2) {
      printf("Usage: %s <COMMAND> <CLIENT FILENAME> <SERVER FILENAME>\n", argv[0]);
      return -1;
  } else if (argc < 5) {
  
    // int socket_desc;
    socket_md_t* client_metadata = create_socket_md();
    if (!client_metadata) {
        printf("Failed to set socket metadata\n");
        return -1;
    }
    struct sockaddr_in server_addr; // https://thelinuxcode.com/sockaddr-in-structure-usage-c/
    
    // Create socket:
    client_metadata->client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(client_metadata->client_sock_fd < 0){
      printf("Unable to create socket\n");
      free_socket(client_metadata);
      return -1;
    }
    
    printf("Socket created successfully\n");
    
    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // Send connection request to server:
    if(connect(client_metadata->client_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
      printf("Unable to connect\n");
      free_socket(client_metadata);
      return -1;
    }
    printf("Connected with server successfully\n");

    send_args_message(client_metadata, argc, argv);

    // Set server socket metadata
    set_client_sock_metadata(client_metadata, argc, argv);

    print_read_file_info(client_metadata); // FIXME: mayeb delete
    

    client_cmd_handler(client_metadata);
    
    // Close the socket:
    free_socket(client_metadata);
  } else {
    printf("Too many arguments\n");
    printf("Usage: %s <COMMAND> <CLIENT FILENAME> <SERVER FILENAME>\n", argv[0]);
    return -1;
  }
  
  
  return 0;
}
