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
#include <pthread.h>
#include <stdbool.h>

#include "config.h"
#include "socket_md.h"
#include "utils.h"


pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;

char client_message[MSG_SIZE];
bool stop_server = false;


// Function to handle the STOP command
void handle_stop(const char* exit_msg) {
    pthread_mutex_lock(&socket_mutex);
    stop_server = true;  // Set the global stop flag
    pthread_mutex_unlock(&socket_mutex);
    printf("%s", exit_msg);
}


/**
 * @brief handles the CLI args commands for the server
 *
 * @param socket socket_t* - the pointer to the server socket metadata object
 */
void* server_cmd_handler(void* arg) {
  socket_md_t* sock = (socket_md_t*)arg;
  if (!sock) {
      fprintf(stderr, "ERROR: Socket is NULL\n");
      return;
  }

  // Clean buffers:
  memset(client_message,'\0',sizeof(client_message));
  char* msg = NULL;
  switch(sock->command) {
    case WRITE:
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
              perror("Failed to send response to client\n");
          }

        break;
    case GET:
        // send the file to client
        send_file(sock, sock->client_sock_fd);
        // Wait for acknowledgment from the other socket before declaring success
        if (recv(sock->client_sock_fd, client_message, sizeof(client_message), 0) < 0) {
            perror("Error receiving acknowledgment from server\n");
            return;
        }
        
        printf("Client's response: %s\n", client_message);
        break;
    case RM:
        const char* rm_obj = sock->first_filepath;
        // if command is RM then check if
        if(rm_file_or_folder(sock) != 1) {
            const char* const_msg = "Failed to remove";
            msg = dyn_msg(const_msg, rm_obj);
        } else {
            const char* const_msg = "Successfully removed";
            msg = dyn_msg(const_msg, rm_obj);
        }

        if (msg != NULL) {
            if (send_msg(sock->client_sock_fd, msg) < 0) {
                perror("Failed to send response to client\n");
            }
            free(msg); // Free the allocated memory
        }
        break;
    case STOP:
        msg = "Exiting Server...\n";
        send_msg(sock->client_sock_fd, msg);
        handle_stop(msg);
        break;
    default:
        printf("Unknown command\n");
        break;
  }
}

/**
 * @brief receives the args message from the Client and distinguishes the CLI args commands into the respective member fields of the socket metadata obj
 *
 * @param socket socket_md_t* - the pointer to the client socket metadata object
 */
void set_srvr_sock_metadata(socket_md_t* sock) {
    // Clean buffers:
    memset(client_message, '\0', sizeof(client_message));
    // Receive client's message:
    
    if (recv(sock->client_sock_fd, client_message, 
            sizeof(client_message), 0) < 0){
      printf("Failed to receive message from client\n");
      close(sock->client_sock_fd);
      return;
    }
    printf("Msg from client: %s\n", client_message);

    // need to process the client msg into separate cmd, local filename, server filename if sending as 1 string
    const char* first_path = NULL;
    int token_count = 0;
    char* token = strtok(client_message, DELIMITER);
    while (token != NULL) {
      if (token_count == 0) {
        commands cmd = str_to_cmd_enum(token);
        set_sock_command(sock, cmd);
      } else if (token_count == 1) {
        set_first_fileInfo(token, sock);
        set_sock_first_filepath(sock);
        first_path = token;
      } else if (token_count == 2) {
        set_sec_fileInfo(token, sock);
        set_sock_sec_filepath(sock);
      }
      token_count++;
      token = strtok(NULL, DELIMITER);
    }

    if (sock->sec_filename == NULL) { // if 3 command is omitted
      set_sec_fileInfo(first_path, sock);
      set_sock_sec_filepath(sock);
    }

}

/**
 * @brief handles the CLI args commands that are pased to it as a message by the client
 *
 * @return int - 0 if success otherwise -1 for error 
 */
int main(void) {
  // int socket_desc, client_sock;
  socket_md_t* server_metadata = create_socket_md();
  if (!server_metadata) {
      printf("Failed to set socket metadata\n");
      return -1;
  }
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  
  // Create socket:
  server_metadata->server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  
  if(server_metadata->server_sock_fd < 0){
    printf("Error while creating socket\n");
    free_socket(server_metadata);
    return -1;
  }
  printf("Socket created successfully\n");
  
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  
  // Bind to the set port and IP:
  if(bind(server_metadata->server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("Couldn't bind to the port\n");
    free_socket(server_metadata);
    return -1;
  }
  printf("Done with binding\n");
  
  // Listen for clients:
  if(listen(server_metadata->server_sock_fd, SERVER_BACKLOG) < 0){
    printf("Error while listening\n");
    free_socket(server_metadata);
    return -1;
  }
  
  while(1) { // loop through to have server continue listening until shut down
    pthread_mutex_lock(&socket_mutex);
    if (stop_server) {
        pthread_mutex_unlock(&socket_mutex);
        break;  // Exit the thread if server is stopping
    }
    pthread_mutex_unlock(&socket_mutex);

    printf("\nListening for incoming connections on port %d\n", PORT);

    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    server_metadata->client_sock_fd = accept(server_metadata->server_sock_fd, (struct sockaddr*)&client_addr, &client_size);
    
    if (server_metadata->client_sock_fd < 0){
      printf("Can't accept\n");
      close(server_metadata->client_sock_fd);
      continue;
    }
    printf("Client connected at IP: %s and port: %i\n", 
          inet_ntoa(client_addr.sin_addr), 
          ntohs(client_addr.sin_port));

    // Inside your main loop
    socket_md_t* client_md = dup_sock_md(server_metadata);
    if (!client_md) {
        printf("Failed to create client socket\n");
        close(server_metadata->client_sock_fd);
        continue;
    }

    // Set server socket metadata
    set_srvr_sock_metadata(client_md);
    
    printf("Command: %s, Send Filename: %s, Receive Filename: %s\n", cmd_enum_to_str(client_md->command), client_md->first_filepath, client_md->sec_filepath);
    print_write_file_info(client_md); // FIXME: maybe delete

    // Create reader and writer threads:
    pthread_t cmd_handle_tid;

    // cmd hanling thread
    if (pthread_create(&cmd_handle_tid, NULL, server_cmd_handler, (void*)client_md) != 0) {
        perror("Failed to create reader thread");
        close(client_md->client_sock_fd);
        continue;
    }

    pthread_detach(cmd_handle_tid); // Detach the thread to manage its own cleanup

    
    // Closing the socket:
    free_socket(client_md);
  }

  // Clean up server socket
  free_socket(server_metadata);
    
  return 0;
}
