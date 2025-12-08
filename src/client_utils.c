
/**
 * @file client_utils.c / source file for Client TCP Socket utilitis.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 */

#include "client_utils.h"

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
client_cmd_handler
*/
void client_cmd_handler(socket_md_t* sock) {
  if (!sock) {
      fprintf(stderr, "ERROR: Socket is NULL\n");
      return;
  }

  int request_sent_status = send_request(sock); // send args
  
  // Clean buffers:
  char server_message[MSG_SIZE]; // Declare server message - since its a stream will send as comma-delimited string
  memset(server_message,'\0',sizeof(server_message));
  // const char* msg = NULL;
  // handle different commands
  char* msg = NULL;
  int sock_fd =sock->client_sock_fd;
  commands cmd = sock->command;
  if (cmd == WRITE) {
      const char* filepath = sock->first_filepath;
      // check if file_exits - if yes then send and receive
      long file_size = get_file_size(filepath);
      sock->file_size = htonl(file_size);
      printf("Client: First file exists with size of %ld.\n", file_size);
    //   printf("Long File Size: %ld\n", file_size);
      if (file_exists(filepath) == 1 && file_size > 0) {
          int request_sent_status = send_request(sock);
          if (request_sent_status == 0) {
              printf("Sending file data (%lu bytes)...\n", file_size);
              if (send_file(filepath, sock_fd) < 0) {
                  fprintf(stderr, "send_file failed\n");
                  return;
              }
              printf("File sent..\n");
          }
      } else {
          fprintf(stderr, "Client: File does not exist.\n");
      }
      
      return;
    } else if (cmd == GET) {
        const char* filepath = sock->sec_filepath;
        rcv_request(sock); // a. receive file size

        ssize_t file_rcvd_bytes = 0;
        pthread_mutex_lock(&file_mutex);  // Lock filesystem
        int folder_exists = folder_not_exists_make(filepath);
        if (folder_exists == 0) {
            printf("Path existed or was newly created\n");

            uint32_t size = sock->file_size;
            printf("Receiving file (%u bytes) to: %s\n", size, filepath);
            file_rcvd_bytes = rcv_file(sock_fd, filepath, size); // b. receive file
            pthread_mutex_unlock(&file_mutex);  // Lock filesystem

            msg = build_send_msg(0,"Client: ", "File received successfully!");
        } else {
            msg = build_send_msg(0, "Client: ", "Error folder was not able to be made");
            if (file_rcvd_bytes < 0 ) {
                msg = build_send_msg(0, "Client: ", "Error receiving file");
            }
        }

        printf("%s\n", msg);

        if (msg != NULL) {
          free(msg);
        }
        return;
    } else if (cmd == RM) {
        if (request_sent_status == 0) {
            // Wait for acknowledgment from the other socket before declaring success
            ssize_t rm_recv = recv(sock_fd, server_message, sizeof(server_message), 0); // receive server's response to handling RM
            if (rm_recv < 0) {
                perror("Error receiving acknowledgment from server");
                return;
            } else if (rm_recv == 0) {
                printf("Server closed connection\n");
                return;
            }
        }

        __print_server_resp(server_message);

      return;
    } else if (cmd == STOP) {
      // Receive the server's response:
      if(recv(sock->client_sock_fd, server_message, sizeof(server_message), 0) < 0) {
        printf("Error while receiving server's msg\n");
        return;
      }
      
      printf("Server's response:\n%s\n",server_message);
      return;
    } else {
      printf("Unknown command\n");
      return;
    }
}


/* set_client_sock_metadata */
void set_client_sock_metadata(socket_md_t* sock, int argc, char* argv[]) {
  // set members of socket object
  set_command(sock, str_to_cmd_enum(argv[1]));

  if (argc > 2) { // set first_filepath
      set_first_fileInfo(argv[2], sock);
      set_first_filepath(sock);
  }
  
  if (argc > 3) { // WRITE - if argv[3] is null then use file name of arfv[2] / GET - if argv[3] is null then need to use default local path
      set_sec_fileInfo(argv[3], sock);
      set_sec_filepath(sock);
  }
}

/*
__print_server_resp
*/
void __print_server_resp(const char* response) {
  printf("Client:\n Server Response: %s\n", response);
}