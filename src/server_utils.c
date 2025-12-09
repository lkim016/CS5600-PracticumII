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


        print_sock_metada(sock); // FIXME: maybe delete

        // Clean buffers:
        char client_message[MSG_SIZE];
        memset(client_message,'\0',sizeof(client_message));
        char* msg = NULL;
        // set values
        commands cmd = sock->command;
        int sock_fd = sock->client_sock_fd;
        char* filepath1 = NULL;
        if (sock->first_filepath != NULL) {
          filepath1 = strdup(sock->first_filepath);
        }
        char* filepath2 = NULL;
        if (sock->sec_filepath != NULL) {
          filepath2 = strdup(sock->sec_filepath);
        }
        uint32_t file_size = 0;
        if (cmd == WRITE) {
                rcv_request(sock); // receive for file size
                file_size = sock->file_size;

                ssize_t file_rcvd_bytes = 0;
                pthread_mutex_lock(&file_mutex);  // Lock filesystem
                int folder_exists = folder_not_exists_make(filepath2);
                if (folder_exists == 0 && file_size > 0) {
                        printf("Server: Path existed or was newly created\n");

                        printf("Server: Receiving file (%u bytes) to: %s\n", file_size, filepath2);
                        file_rcvd_bytes = rcv_file(sock_fd, filepath2, file_size);
                        pthread_mutex_unlock(&file_mutex);  // Lock filesystem
                        if (file_rcvd_bytes < 0 ) {
                            msg = build_send_msg(threadID, "Server: ", "Error receiving file");
                        }

                        msg = build_send_msg(threadID,"Server: ", "File sent successfully!");
                } else {
                        if(file_size == 0) {
                            msg = build_send_msg(threadID, "Server: ", "Error size of file being sent is 0");
                        } else {
                            msg = build_send_msg(threadID, "Server: ", "Error file size is 0 or folder was not able to be made");
                        }
                }

            // if (send_msg(sock_fd, msg) < 0) {
            //     perror("Failed to send response to client\n");
            // }

              printf("%s\n", msg);

              if (msg != NULL) {
                free(msg);
              }

              if (filepath1 != NULL) {
                  free(filepath1);
              }
              if (filepath2 != NULL) {
                  free(filepath2);
              }

              return NULL;
        } else if (cmd == GET) {
            // check if file_exits - if yes then send and receive
            sock->file_size = get_file_size(filepath1);
            file_size = sock->file_size;
            printf("Server: First file exists with size of %u.\n", file_size);
            if (file_exists(filepath1) == 1 && file_size > 0) {
                int size_sent_status = send_size(sock_fd, file_size);
                if (size_sent_status == 0) {
                    printf("Server: Sending file data (%u bytes)...\n", file_size);
                    if (send_file(filepath1, sock_fd) <= 0) { // b. send file
                        msg = build_send_msg(threadID, "Server: ", "send_file failed");
                    } else {
                        msg = build_send_msg(threadID, "Server: ", "File sent..");
                    }
                }
            } else {
                msg = build_send_msg(threadID, "Server: ", "File does not exist.");
            }

            printf("%s\n", msg);

            if (msg != NULL) {
              free(msg);
            }
            if (filepath1 != NULL) {
                free(filepath1);
            }
            if (filepath2 != NULL) {
                free(filepath2);
            }

            return NULL;
          } else if (cmd == RM) {
            if (filepath1 != NULL) {
              const char* rm_item = filepath1;
              // need to lock here since this is modifying a folder or file
              pthread_mutex_lock(&file_mutex);
              int rm_status = rm_file_or_folder(sock);
              pthread_mutex_unlock(&file_mutex);
              if(rm_status != 1) {
                  const char* const_msg = "Server\n RM: Failed to remove";
                  msg = build_send_msg(threadID, const_msg, rm_item);
              } else {
                  const char* const_msg = "Server\n RM: Successfully removed";
                  msg = build_send_msg(threadID, const_msg, rm_item);
              }
            } else {
                const char* const_msg = "Server\n RM: First filepath is NULL";
                msg = build_send_msg(threadID, const_msg, "");
            }
            
            printf("%s\n", msg);

            if (msg != NULL) {
                if (send_msg(sock->client_sock_fd, msg) < 0) { // send response to handling RM
                    perror("Server: Failed to send server response to client\n");
                }

                free(msg); // Free the allocated memory
            }

            if (filepath1 != NULL) {
                free(filepath1);
            }
            if (filepath2 != NULL) {
                free(filepath2);
            }
            return NULL;
        } else if (cmd == STOP) {
            msg = build_send_msg(threadID, "Exiting Server...", "");
            printf("%s\n", msg);
            ssize_t msg_send_bytes = send_msg(sock->client_sock_fd, msg);
            if (msg_send_bytes > 0) {
              printf("Server: final message was sent to client\n");
            }

            // Add delay to ensure client receives
            // usleep(60000);  // 10ms delay

            /*
            pthread_mutex_lock(&stop_mutex);
            stop_server = true;  // Set the global stop flag
            pthread_mutex_unlock(&stop_mutex);
            */
            
            if (msg != NULL) {
              free(msg);
            }
            free_socket(sock);
            if (filepath1 != NULL) {
                free(filepath1);
            }
            if (filepath2 != NULL) {
                free(filepath2);
            }
            exit(0);
          } else {
              printf("Server: Unknown command\n");
          }

        printf("Thread ID %lu ending..\n", (unsigned long)thread_id);

        // Clean and Closing the socket:
        if (filepath1 != NULL) {
            free(filepath1);
        }
        if (filepath2 != NULL) {
            free(filepath2);
        }

        free_socket(sock);
        return NULL;
}


/*
set_server_sock_metadata
*/
/*
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
*/


/*
__print_client_resp
*/
void __print_client_resp(char* response) {
  printf("Server's response:\n%s\n", response);
}