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


pthread_mutex_t stop_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
bool stop_server = false;

/**
 * @brief function to globally handle the STOP command for all threads
 *
 * @param exit_msg char* - a const char string that holds the server STOP message
 */
void handle_stop(const char* exit_msg) {
    pthread_mutex_lock(&stop_mutex);
    stop_server = true;  // Set the global stop flag
    pthread_mutex_unlock(&stop_mutex);
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
      return NULL;
  }

  pthread_t thread_id = pthread_self();
  set_thread_id(sock, (unsigned long)thread_id);
  printf("Thread ID %lu starting..\n", (unsigned long)thread_id);

  print_write_file_info(sock); // FIXME: maybe delete
  // Clean buffers:
  char client_message[MSG_SIZE];
  memset(client_message,'\0',sizeof(client_message));
  char* msg = NULL;
  switch(sock->command) {
    case WRITE:
        pthread_mutex_lock(&file_mutex);  // Lock filesystem
        int folder_exists = folder_not_exists_make(sock->sec_filepath);
        if (folder_exists == 0) {
            // if folder does not exist make if it does then check if file exists
            // if it does then need to get file and rename it to a timestamped version
            int rcvd_status = rcv_file(sock, sock->client_sock_fd);
            
            if (rcvd_status < 0 ) {
              msg = dyn_msg(sock->thread_id, "Error receiving file", "");
            } else {
              msg = dyn_msg(sock->thread_id,"File sent successfully!", "");
            }
          } else {
              msg = dyn_msg(sock->thread_id, "Warning: File was not received. Issue with folder path to write out to.", "");
          }
          pthread_mutex_unlock(&file_mutex);  // Lock filesystem

          printf("%s\n", msg);
          if (send_msg(sock->client_sock_fd, msg) < 0) {
              perror("Failed to send response to client\n");
          }
          // Add delay to ensure client 

          if (msg != NULL) {
            free(msg);
          }

        break;
    case GET:
        pthread_mutex_lock(&file_mutex);  // Lock filesystem
        // send the file to client
        int send_status = send_file(sock, sock->client_sock_fd);
        pthread_mutex_unlock(&file_mutex);  // Lock filesystem
        if (send_status == 0) {
          // Wait for acknowledgment from the other socket before declaring success
          if (recv(sock->client_sock_fd, client_message, sizeof(client_message), 0) < 0) {
              perror("Error receiving acknowledgment from client\n");
              return NULL;
          }
          
          printf("Client's response: %s\n", client_message);
        }
        break;
    case RM:
        if (sock->first_filepath != NULL) {
          const char* rm_item = sock->first_filepath;
          // need to lock here since this is modifying a folder or file
          pthread_mutex_lock(&file_mutex);
          int rm_status = rm_file_or_folder(sock);
          pthread_mutex_unlock(&file_mutex);
          if(rm_status != 1) {
              const char* const_msg = "Failed to remove";
              msg = dyn_msg(sock->thread_id, const_msg, rm_item);
          } else {
              const char* const_msg = "Successfully removed";
              msg = dyn_msg(sock->thread_id, const_msg, rm_item);
          }
        } else {
            const char* const_msg = "First filepath is invalid";
            msg = dyn_msg(sock->thread_id, const_msg, "");
        }
        
        printf("%s\n", msg);

        if (msg != NULL) {
            if (send_msg(sock->client_sock_fd, msg) < 0) {
                perror("Failed to send response to client\n");
            }

            // // Add delay to ensure client receives
            // usleep(10000);  // 10ms delay

            free(msg); // Free the allocated memory
        }
        break;
    case STOP:
        msg = dyn_msg(sock->thread_id, "Exiting Server...", "");
        send_msg(sock->client_sock_fd, msg);

        // Add delay to ensure client receives
        usleep(10000);  // 10ms delay

        handle_stop(msg);
        if (msg != NULL) {
          free(msg);
        }
        break;
    default:
        printf("Unknown command\n");
        break;
  }
  
  printf("Thread ID %lu ending..\n", (unsigned long)thread_id);

  // Clean and Closing the socket:
  free_socket(sock);
  sock = NULL;
  return NULL;
}


/**
 * @brief receives the args message from the Client and distinguishes the CLI args commands into the respective member fields of the socket metadata obj
 *
 * @param socket socket_md_t* - the pointer to the client socket metadata object
 */
void set_server_sock_metadata(socket_md_t* sock) {
    // Clean buffers:
    char client_message[MSG_SIZE];
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
    char* token = strtok(client_message, DELIM);
    while (token != NULL) {
      if (token_count == 0) {
        commands cmd = str_to_cmd_enum(token);
        set_command(sock, cmd);
      } else if (token_count == 1) {
        first_path = token;
        set_first_fileInfo(token, sock);
        set_first_filepath(sock);
      } else if (token_count == 2) {
        set_sec_fileInfo(token, sock);
        set_sec_filepath(sock);
      }
      token_count++;
      token = strtok(NULL, DELIM);
    }

    if (sock->sec_filename == NULL) { // if 3 command is omitted
      set_sec_fileInfo(first_path, sock);
      set_sec_filepath(sock);
    }

}

/**
 * @brief handles the CLI args commands that are pased to it as a message by the client
 *
 * @return int - 0 if success otherwise -1 for error 
 */
int main(void) {
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  
  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  
  if(socket_desc < 0){
    printf("Error while creating socket\n");
    close(socket_desc);
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
    close(socket_desc);
    return -1;
  }
  printf("Done with binding\n");
  
  // Listen for clients:
  if(listen(socket_desc, SERVER_BACKLOG) < 0){
    printf("Error while listening\n");
    close(socket_desc);
    return -1;
  }
  
  while(1) { // loop through to have server continue listening until shut down
    pthread_mutex_lock(&stop_mutex);
    bool should_stop = stop_server;
    pthread_mutex_unlock(&stop_mutex);

    if (should_stop) {
        break;  // Exit the thread if server is stopping
    }
    
    printf("\nListening for incoming connections on port %d\n", PORT);
    
    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    
    if (client_sock < 0){
      printf("Can't accept\n");
      continue;
    }
    
    // create a new socket metadata for every client connection
    socket_md_t* server_metadata = create_socket_md(client_sock);
    if (!server_metadata) {
      printf("Failed to create socket metadata\n");
      continue;
    }

    set_server_sock_fd(server_metadata, socket_desc);

    printf("Client connected at IP: %s and port: %i\n", 
          inet_ntoa(client_addr.sin_addr), 
          ntohs(client_addr.sin_port));

    // Set server socket metadata
    set_server_sock_metadata(server_metadata);
    
    printf("Command: %s, Send Filename: %s, Receive Filename: %s\n", cmd_enum_to_str(server_metadata->command), server_metadata->first_filepath, server_metadata->sec_filepath);

    // Create reader and writer threads:
    pthread_t thread;

    // cmd hanling thread
    if (pthread_create(&thread, NULL, server_cmd_handler, (void*)server_metadata) != 0) {
        perror("Failed to create reader thread");
        close(server_metadata->client_sock_fd);
        continue;
    }

    pthread_detach(thread); // Detach the thread to manage its own cleanup

  }

  close(socket_desc);
    
  return 0;
}
