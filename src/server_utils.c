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
        commands cmd = sock->command;
        // COMMANDS
        if (cmd == WRITE) {
                printf("Thread ID: %lu\n", t_id);
                
                printf("Expected File Size: %u\n", sock->file_size);
                msg = receive(t_id, sock);

                printf("%s\n", msg);

                if (msg != NULL) {
                    free(msg);
                }

                return;
        } else if (cmd == GET) {
            printf("Thread ID: %lu\n", t_id);
            // check if file_exits - if yes then send and receive
            sock->file_size = get_file_size(sock->first_filepath);

            msg = deliver(t_id, sock);

            printf("%s\n", msg);

            if (msg != NULL) {
                free(msg);
            }

            return;
          } else if (cmd == RM) {
                printf("Thread ID: %lu\n", t_id);
                
            if (sock->first_filepath != NULL) {
              // need to lock here since this is modifying a folder or file
              pthread_mutex_lock(&rm_file_mutex);
              int rm_status = rm_file_or_folder(sock);
              pthread_mutex_unlock(&rm_file_mutex);
              if(rm_status != 1) {
                  const char* const_msg = "RM: Failed to remove";
                  msg = build_send_msg(t_id, const_msg, sock->first_filepath);
              } else {
                  const char* const_msg = "Server\nRM: Successfully removed";
                  msg = build_send_msg(t_id, const_msg, sock->first_filepath);
              }
            } else {
                const char* const_msg = "Server\nRM: First filepath is NULL";
                msg = build_send_msg(t_id, const_msg, sock->first_filepath);
            }
            
            printf("%s\n", msg);

            if (msg != NULL) {
                if (send_msg(sock->client_sock_fd, msg) < 0) { // send response to handling RM
                    perror("Server: Failed to send server response to client\n");
                }

                free(msg); // Free the allocated memory
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

            if (msg != NULL) {
                free(msg);
            }
            
            free_socket(sock);

            signal(SIGINT, handle_sigint);
            exit(0);

            return; // FIXME: change when multi-threading
          } else {
              printf("Server: Unknown command\n");
          }

        printf("Thread ID %lu ending..\n", (unsigned long)thread_id);

}


/*
__print_client_resp
*/
void __print_client_resp(char* response) {
  printf("Server's response:\n%s\n", response);
}