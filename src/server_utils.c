/**
 * @file server_utils.c / source file for Server TCP Socket utilitis.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 */

#include "server_utils.h"

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;


/*
dyn_msg
NOTE: need to free() result
*/
char* build_send_msg(unsigned long id, const char* part1, const char* part2) { // FIXME: change to build_send_msg
    size_t len = strlen(part1) + strlen(part2) + 3; // 1 for space, 1 for newline, 1 for null terminator
    char* msg_ptr = calloc(len, sizeof(char));
    if (msg_ptr == NULL) {
        perror("Memory allocation failed\n");
        return NULL;
    }
    sprintf(msg_ptr, "Thread ID#%lu:\n  %s %s\n", id, part1, part2);
    return msg_ptr;
}

/*
server_cmd_handler
*/
/*
void* server_cmd_handler(void* arg) {
  socket_md_t* sock = (socket_md_t*)arg;
  if (!sock) {
      fprintf(stderr, "ERROR: Socket is NULL\n");
      return NULL;
  }
*/
void* server_cmd_handler(socket_md_t* sock) {
  pthread_t thread_id = pthread_self();
  printf("Thread ID %lu starting..\n", (unsigned long)thread_id);
  unsigned long threadID = (unsigned long)thread_id;


  // Clean buffers:
  char client_message[MSG_SIZE];
  memset(client_message,'\0',sizeof(client_message));
  char* msg = NULL;
  switch(sock->command) {
    case WRITE:
        const char* filepath = sock->sec_filepath;
        int sock_fd = sock->client_sock_fd;

        pthread_mutex_lock(&file_mutex);  // Lock filesystem
        int folder_exists = folder_not_exists_make(filepath);
        if (folder_exists == 0) {
            printf("Path was created\n");

        }
        pthread_mutex_unlock(&file_mutex);  // Lock filesystem

        uint32_t size = sock->file_size;
        char* sec_filepath = sock->sec_filepath;
        printf("Receiving file (%u bytes) to: %s\n", size, sec_filepath);
        ssize_t file_rcvd_bytes = rcv_file(sock_fd, sec_filepath, size);

        msg = build_send_msg(threadID,"Server: File sent successfully!", "");
        
        // if (file_rcvd_bytes < 0 ) {
        //       msg = build_send_msg(threadID, "Server: Error receiving file", "");
        //     } else {
              
        //     }
        // } else {
        //       if (folder_exists < 0) {
        //           msg = build_send_msg(threadID, "Server: Error folder was not able to be made", "");
        //       }
        //       msg = build_send_msg(threadID, "Server: Error file size sent is 0", "");

        // if (file_size > 0) { // && folder_exists == 0) {
        //     // if file_size received is valid
        //     // if it does then need to get file and rename it to a timestamped version
        //     ssize_t file_rcvd_bytes = rcv_file(filepath, sock_fd, file_size);
            
        // }

        // if (send_msg(sock_fd, msg) < 0) {
        //     perror("Failed to send response to client\n");
        // }

        printf("%s\n", msg);

        if (msg != NULL) {
          free(msg);
        }
            
        break;
    /*
    case GET:
        pthread_mutex_lock(&file_mutex);  // Lock filesystem
        // send the file to client
        int send_status = send_file(sock, sock->client_sock_fd);
        pthread_mutex_unlock(&file_mutex);  // Lock filesystem
        if (send_status == 0) {
          // Wait for acknowledgment from the other socket before declaring success
          if (recv(sock->client_sock_fd, client_message, sizeof(client_message), 0) < 0) {
              perror("Error receiving acknowledgment from client\n");
              // return NULL;
              return;
          }
          
          print_client_resp(client_message);
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
              msg = build_send_msg(threadID, const_msg, rm_item);
          } else {
              const char* const_msg = "Successfully removed";
              msg = build_send_msg(threadID, const_msg, rm_item);
          }
        } else {
            const char* const_msg = "First filepath is invalid";
            msg = build_send_msg(threadID, const_msg, "");
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
    */
    case STOP:
        msg = build_send_msg(threadID, "Exiting Server...", "");
        ssize_t msg_send_bytes = send_msg(sock->client_sock_fd, msg);

        // Add delay to ensure client receives
        usleep(10000);  // 10ms delay
        /*
        pthread_mutex_lock(&stop_mutex);
        stop_server = true;  // Set the global stop flag
        pthread_mutex_unlock(&stop_mutex);
        */
        printf("%s", msg);
        
        if (msg != NULL) {
          free(msg);
        }
        free_socket(sock);
        exit(0);
        break;
    default:
        printf("Unknown command\n");
        break;
  }
  
  printf("Thread ID %lu ending..\n", (unsigned long)thread_id);

  // Clean and Closing the socket:
  free_socket(sock);
  return NULL;
}

/*
rcv_args_message
NOTE: free() ptr after use
*/
/*
char* rcv_args_message(int sock_fd) {
    if (sock_fd <= 0) {
        fprintf(stderr, "server's socket metadata is NULL\n");
        return NULL;
    }
    
    // Clean buffers:
    char client_message[MSG_SIZE];
    memset(client_message, '\0', sizeof(client_message));
    // Receive client's message:
    
    ssize_t received_len = recv(sock_fd, client_message, sizeof(client_message), 0);
    if (received_len < 0){
      printf("Failed to receive message from client\n");
      return NULL;
    }

    char* message = strdup(client_message);
    if (!message) {
        perror("malloc failed for server_message\n");
        return NULL;
    }
    ssize_t ready_sent_bytes = send_msg(sock_fd, "Command Message Received\n");
    printf("Msg from client: %s\n", message);
    return message;
}
    */


/*
set_server_sock_metadata
*/
void set_server_sock_metadata(socket_md_t* sock, char* message) {

    if(sock == NULL) {
      fprintf(stderr, "server socket metadata is NULL");
      return;

    }
    if (message == NULL) {
        fprintf(stderr, "message received from client is NULL\n");
        return;
    }

    // need to process the client msg into separate cmd, local filename, server filename if sending as 1 string
    const char* first_path = NULL;
    int token_count = 0;
    char* token = strtok(message, DELIM);
    while (token != NULL) {
      printf("%s\n", token);
      if (token_count == 0) {
        commands cmd = str_to_cmd_enum(token);
        if (cmd == NULL_VAL) {
          fprintf(stderr, "Error: invalid command received\n");
        }
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

    if (sock->sec_filename == NULL) { // if 3rd command is omitted
      if (first_path == NULL) {
        fprintf(stderr, "ERROR: first path is also NULL\n");
      }
      set_sec_fileInfo(first_path, sock);
      set_sec_filepath(sock);
    }
    
    free(message);

    return;

}


/*
__print_client_resp
*/
void __print_client_resp(char* response) {
  printf("Server's response:\n%s\n", response);
}