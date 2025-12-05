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
      exit(1);
  }
  const char *command = args[1];
  // send command WRITE, GET, RM along with the file
  int msg_size = 0;
  if(strcmp(command, "WRITE") == 0) {
    
    if (args[2] == NULL) {
      printf("Invalid file path\n");
      exit(1);
    }
    
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
    //------
    int fp_size = strlen(LOCAL_FILE_PATH) + strlen(args[2]) + 1;
    char file_path[fp_size]; // ex: data/file.txt
    sprintf(file_path, "%s%s", LOCAL_FILE_PATH, args[2]);
    printf("Local File path: %s\n", file_path);
    FILE *file = fopen(file_path, "rb"); // "rb" for read binary
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Get the size of the file
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error seeking to end of file");
        fclose(file);
        exit(1);
    }
    long file_size = ftell(file);
    if(file_size < 0) {
        perror("Error getting file size");
        fclose(file);
        exit(1);

    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("Error seeking to start of file");
        fclose(file);
        exit(1);
    }
    

    // send the file size
    uint32_t size = htonl(file_size);
    send(socket, &size, sizeof(size), 0);

    // send the file data
    char buffer[CHUNK_SIZE]; // buffer to hold file chunks
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) { // reads the given amount of data (CHUNK_SIZE) from the file into the buffer
      size_t total_sent = 0;

      while (total_sent < bytes_read) {
          ssize_t sent = send(socket, buffer + total_sent,
                              bytes_read - total_sent, 0);

          if (sent < 0) {
              perror("Unable to send message\n");
              // handle error (disconnect, etc.)
              exit(1);
          }

          total_sent += sent;
        }
    }
    
    fclose(file);

    return;
  }
    /*
    else if(strcmp(command, "GET") == 0) { // For GET, no file data is sent but still need to send the command, filename, and 
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

 close(socket);

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
    
    // Get input from the user:
    /*
    printf("Enter message: ");
    gets(client_message);
    */
   
    // FIXME: maybe do the command handling here
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
