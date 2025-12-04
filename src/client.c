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


void send_socket(int socket, int arg_count, char* args[]) {
  // const char *command, const char *file_path
  if (args[1] == NULL) {
      printf("Invalid command\n");
      return;
  }
  const char *command = args[1];
  // send command WRITE, GET, RM along with the file
  int msg_size = 0;
  if(strcmp(command, "WRITE") == 0) {
    
    if (args[2] == NULL) {
      printf("Invalid file path\n");
      return;
    }

    const char *file_path = args[2];
    FILE *file = fopen(file_path, "rb"); // "rb" for read binary
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Get the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Calculate message length when combined by delimiters - command,filename,file_size,remote_filename
    for(int i = 1; i < arg_count; i++) {
      msg_size += strlen(args[i]);
      msg_size += 1; // for comma or null terminator
    }
    
    // Declare client message
    char client_message[msg_size];
    // Clean buffer:
    memset(client_message,'\0',sizeof(client_message));
    
    // Construct the message with delimiters
    // example: ./rfs WRITE data/file.txt remote/file.txt
    // becomes: WRITE, data/file.txt, remote/file.txt
    for(int i = 1; i < arg_count; i++) {
      strcat(client_message, args[i]);
      if (i < arg_count - 1) {
          strcat(client_message, DELIMITER);
      }
    }
    if(send(socket, client_message, strlen(client_message), 0) < 0){
      printf("Unable to send message\n");
      close(socket);
      return;
    }

    /*
    // Allocate memory to hold the file data
    uint8_t *file_data = (uint8_t *)malloc(file_size);
    if (file_data == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }
    // Read the file data into the buffer
    fread(file_data, 1, file_size, file);
    // Send the message using send_message function
    if(send(socket, client_message, strlen(client_message), 0) < 0){
      printf("Unable to send message\n");
      close(socket);
      return;
    }
    */
    // free(file_data);
    fclose(file);
    return;
  }
    /*
    else if(strcmp(command, "WRITE") == 0) { // For GET, no file data is sent but still need to send the command, filename, and 
      break;
    case "RM":
      // For GET and RM, no file data is sent
      if (send_message(socket, client_message, strlen(client_message), NULL, 0) < 0) {
          printf("Failed to send message\n");
      }
    */
  else if (strcmp(command, "STOP") == 0) {
      if(send(socket, command, strlen(command), 0) < 0){
        printf("Unable to send message\n");
        close(socket);
        return;
      }
    } else {
      printf("Unknown command\n");
      return;
    }
  
  // memset(client_message,'\0',sizeof(client_message));
  // sprintf(client_message, "%s\n", file_path);

  // Send the file contents
  // fseek(file, 0, SEEK_END);
  // long file_size = ftell(file);
  // rewind(file);
  /*
  char data[SIZE];
  while (fgets(data, SIZE, fp) != NULL) {
      send(socket, data, sizeof(data), 0);
      memset(data, '\0', SIZE);
  }
  
  // Send the size of the file
  send(socket, &file_size, sizeof(file_size), 0);
  
  // Send the file data in chunks
  char buffer[1024];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
      send(socket, buffer, bytes_read, 0);
  }
  */

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
      printf("Usage: %s <COMMAND> <FILENAME>\n", argv[0]);
      return -1;
  } else if (argc < 3 && (strcmp(argv[1], "WRITE") == 0 || strcmp(argv[1], "GET") == 0 || strcmp(argv[1], "RM") == 0)) {
      printf("Usage: %s %s <FILENAME>\n", argv[0], argv[1]);
      return -1;
  } else if ((argc >= 3 && argc < 5) || strcmp(argv[1], "STOP") == 0) {
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
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Send connection request to server:
    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
      printf("Unable to connect\n");
      close(socket_desc);
      return -1;
    }
    printf("Connected with server successfully\n");
    
    // Get input from the user:
    /*
    printf("Enter message: ");
    gets(client_message);
    */
   
    // Send the message to server:
    send_socket(socket_desc, argc, argv);
    
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
