/**
 * @file server_utils.c / source file for Server TCP Socket utilitis.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 */

#include "server_utils.h"

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;


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


  // Add delay to ensure client receives
  // usleep(60000);  // 10ms delay
  // receive request from client and initialize the socket metadata
  rcv_request(sock); // receive args

  print_sock_metada(sock); // FIXME: maybe delete

  // Clean buffers:
  char client_message[MSG_SIZE];
  memset(client_message,'\0',sizeof(client_message));
  char* msg = NULL;
  int sock_fd = sock->client_sock_fd;
  const char* filepath = NULL;
  commands cmd = sock->command;

  if (cmd == WRITE) {
      filepath = sock->sec_filepath;
      rcv_request(sock); // receive for file size
      uint32_t size = sock->file_size;
      ssize_t file_rcvd_bytes = 0;
      pthread_mutex_lock(&file_mutex);  // Lock filesystem
      int folder_exists = folder_not_exists_make(filepath);
      if (folder_exists == 0) {
          printf("Path existed or was newly created\n");

          printf("Receiving file (%u bytes) to: %s\n", size, filepath);
          file_rcvd_bytes = rcv_file(sock_fd, filepath, size);
          pthread_mutex_unlock(&file_mutex);  // Lock filesystem

          msg = build_send_msg(threadID,"Server: ", "File sent successfully!");
      } else {
          msg = build_send_msg(0, "Client: ", "Error folder was not able to be made");
          if (file_rcvd_bytes < 0 ) {
              msg = build_send_msg(0, "Client: ", "Error receiving file");
          }
      }

      // if (send_msg(sock_fd, msg) < 0) {
      //     perror("Failed to send response to client\n");
      // }

      printf("%s\n", msg);

      if (msg != NULL) {
        free(msg);
      }

      return NULL;
    } else if (cmd == GET) {
      filepath = sock->first_filepath;
      // check if file_exits - if yes then send and receive
      long file_size = get_file_size(filepath);
      sock->file_size = htonl(file_size);
      printf("Server: File exists with size of %ld.\n", file_size);
      if (file_exists(filepath) == 1 && file_size > 0) {
          int request_sent_status = send_request(sock); // a. send ith file size
          if (request_sent_status == 0) {
              printf("Sending file data (%lu bytes)...\n", file_size);
              if (send_file(filepath, sock_fd) < 0) { // b. send file
                  fprintf(stderr, "send_file failed\n");
                  return NULL;
              }
              printf("File sent..\n");
          }
      } else {
          fprintf(stderr, "Client: File does not exist.\n");
      }

      return NULL;
    } else if (cmd == RM) {
      filepath = sock->first_filepath;
      if (filepath != NULL) {
        const char* rm_item = filepath;
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
          if (send_msg(sock->client_sock_fd, msg) < 0) { // send response to handling RM
              perror("Failed to send response to client\n");
          }

          free(msg); // Free the allocated memory
      }
      return NULL;
    } else if (cmd == STOP) {
      msg = build_send_msg(threadID, "Exiting Server...", "");
      ssize_t msg_send_bytes = send_msg(sock->client_sock_fd, msg);


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
    } else {
      printf("Unknown command\n");
      return NULL;
  }
  
  printf("Thread ID %lu ending..\n", (unsigned long)thread_id);

  // Clean and Closing the socket:
  free_socket(sock);
  return NULL;
}


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