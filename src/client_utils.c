
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

    print_sock_metada(sock); // FIXME: maybe delete
    
    // Clean buffers:
    char server_message[MSG_SIZE]; // Declare server message - since its a stream will send as comma-delimited string
    memset(server_message,'\0',sizeof(server_message));
    // const char* msg = NULL;
    // handle different commands
    char* msg = NULL;
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
        // check if file_exits - if yes then send and receive
        sock->file_size = get_file_size(filepath1);
        file_size = sock->file_size;
        printf("Client: First file exists with size of %u.\n", file_size);
        if (file_exists(filepath1) == 1 && file_size > 0) {
            int size_sent_status = send_size(sock_fd, file_size);
            if (size_sent_status == 0) {
                printf("Client: Sending file data (%u bytes)...\n", file_size);
                if (send_file(filepath1, sock_fd) < 0) {
                    fprintf(stderr, "Client: send_file failed\n");
                } else {
                    printf("Client: File sent..\n");
                }
            }
        } else {
            fprintf(stderr, "Client: File does not exist.\n");
        }
        if (filepath1 != NULL) {
            free(filepath1);
        }
        
        if (filepath2 != NULL) {
            free(filepath2);
        }
        return;
    } else if (cmd == GET) {
        rcv_request(sock); // a. receive file size
        file_size = sock->file_size;

        ssize_t file_rcvd_bytes = 0;
        pthread_mutex_lock(&file_mutex);  // Lock filesystem
        int folder_exists = folder_not_exists_make(filepath2);
        if (folder_exists == 0 && file_size > 0) {
            printf("Client: Path existed or was newly created\n");

            printf("Client: Receiving file (%u bytes) to: %s\n", file_size, filepath2);
            file_rcvd_bytes = rcv_file(sock_fd, filepath2, file_size);
            pthread_mutex_unlock(&file_mutex);  // Lock filesystem
            if (file_rcvd_bytes < 0 ) {
                msg = "Client: Error receiving file";
            }

            msg = "Client: File sent successfully!";
        } else {
            if(file_size == 0) {
                msg = "Client: Error size of file being sent is 0";
            } else {
                msg = "Client: Error file size is 0 or folder was not able to be made";
            }
        }

        printf("%s\n", msg);
        
        if (filepath1 != NULL) {
            free(filepath1);
        }
        
        if (filepath2 != NULL) {
            free(filepath2);
        }
        return;
    } else if (cmd == RM) {
        // Wait for acknowledgment from the other socket before declaring success
        ssize_t rm_recv = recv(sock_fd, server_message, sizeof(server_message), 0); // receive server's response to handling RM
        if (rm_recv < 0) {
            perror("Client: Error receiving acknowledgment from server");
            if (filepath1 != NULL) {
                free(filepath1);
            }
            
            if (filepath2 != NULL) {
                free(filepath2);
            }
            return;
        } else if (rm_recv == 0) {
            printf("Client: Server closed connection\n");
            if (filepath1 != NULL) {
                free(filepath1);
            }
            
            if (filepath2 != NULL) {
                free(filepath2);
            }
            return;
        }

        __print_server_resp(server_message);
        
        if (filepath1 != NULL) {
            free(filepath1);
        }
        
        if (filepath2 != NULL) {
            free(filepath2);
        }

        return;
    } else if (cmd == STOP) {
        // Receive the server's response:
        if(recv(sock_fd, server_message, sizeof(server_message), 0) < 0) {
            printf("Client: Error while receiving server's msg\n");
            if (filepath1 != NULL) {
                free(filepath1);
            }
            
            if (filepath2 != NULL) {
                free(filepath2);
            }
            return;
        }
        
        printf("Client:\n Server's response:\n%s\n",server_message);
        if (filepath1 != NULL) {
            free(filepath1);
        }
        
        if (filepath2 != NULL) {
            free(filepath2);
        }
        return;
    } else {
        printf("Client: Unknown command\n");
    }

    if (filepath1 != NULL) {
        free(filepath1);
    }
    
    if (filepath2 != NULL) {
        free(filepath2);
    }
}


/* set_client_sock_metadata */
void set_client_sock_metadata(socket_md_t* sock, int argc, char* argv[]) {
    // set members of socket object
    if (argv[1] == NULL) {
        fprintf(stderr, "Client: command is missing\n");
    } else {
        set_command(sock, str_to_cmd_enum(argv[1]));
    }

    if (sock->command == STOP) {
        return;
    }
    if (argv[2] == NULL) {
        fprintf(stderr, "Client: filepath1 is missing\n");
    } else {
        set_first_fileInfo(argv[2], sock);
        set_first_filepath(sock);
    }
    if (sock->command == RM) {
        return;
    }
    // WRITE - if argv[3] is null then use file name of arfv[2] / GET - if argv[3] is null then need to use default local path
    if (argv[3] != NULL) {
        set_sec_fileInfo(argv[3], sock);
        set_sec_filepath(sock);
    } else {
        fprintf(stderr, "Client: filepath2 is missing\n");
    }
}

/*
__print_server_resp
*/
void __print_server_resp(const char* response) {
  printf("Client:\n Server Response: %s\n", response);
}