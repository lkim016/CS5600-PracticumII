/**
 * @file server_utils.c / source file for Server TCP Socket utilitis.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 */

#include "server_utils.h"

pthread_mutex_t rm_file_mutex = PTHREAD_MUTEX_INITIALIZER;

volatile sig_atomic_t shutting_down = 0;

/*
server_cmd_handler
*/
void handle_sigint(int sig) {
    shutting_down = 1;
}


void server_cmd_handler(socket_md_t* sock) {
        pthread_t thread_id = pthread_self();
        printf("Thread ID %lu starting..\n", (unsigned long)thread_id);
        unsigned long t_id = (unsigned long)thread_id;

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
        // COMMANDS
        if (cmd == WRITE) {
                printf("Thread ID: %lu\n", t_id);
                rcv_request(sock); // receive for file size
                file_size = sock->file_size;
                
                printf("Expected File Size: %u\n", file_size);
                msg = deliver(t_id, sock_fd, filepath2, file_size);

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

                return;
        } else if (cmd == GET) {
            printf("Thread ID: %lu\n", t_id);
            // check if file_exits - if yes then send and receive
            sock->file_size = get_file_size(filepath1);
            file_size = sock->file_size;

            msg = receive(t_id, sock_fd, filepath1, file_size);

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

            return;
          } else if (cmd == RM) {
            printf("Thread ID: %lu\n", t_id);
            if (filepath1 != NULL) {
              const char* rm_item = filepath1;
              // need to lock here since this is modifying a folder or file
              pthread_mutex_lock(&rm_file_mutex);
              int rm_status = rm_file_or_folder(sock);
              pthread_mutex_unlock(&rm_file_mutex);
              if(rm_status != 1) {
                  const char* const_msg = "Server\n RM: Failed to remove";
                  msg = build_send_msg(t_id, const_msg, rm_item);
              } else {
                  const char* const_msg = "Server\n RM: Successfully removed";
                  msg = build_send_msg(t_id, const_msg, rm_item);
              }
            } else {
                const char* const_msg = "Server\n RM: First filepath is NULL";
                msg = build_send_msg(t_id, const_msg, "");
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
            return;
        } else if (cmd == STOP) {
            msg = build_send_msg(t_id, "Exiting Server...", "");
            printf("%s\n", msg);
            ssize_t msg_send_bytes = send_msg(sock->client_sock_fd, msg);
            if (msg_send_bytes > 0) {
              printf("Server: final message was sent to client\n");
            }

            // Add delay to ensure client receives
            // usleep(60000);  // 10ms delay

            signal(SIGINT, handle_sigint);
            
            if (msg != NULL) {
                free(msg);
            }
            if (filepath1 != NULL) {
                free(filepath1);
            }
            if (filepath2 != NULL) {
                free(filepath2);
            }
            free_socket(sock);
            return; // FIXME: change when multi-threading
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

}


/*
__print_client_resp
*/
void __print_client_resp(char* response) {
  printf("Server's response:\n%s\n", response);
}