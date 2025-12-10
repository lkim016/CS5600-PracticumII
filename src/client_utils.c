
/**
 * @file client_utils.c / source file for Client TCP Socket utilitis.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 */

#include "client_utils.h"


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
    // COMMANDS
    if (cmd == WRITE) {
        sock->file_size = get_file_size(filepath1);
        file_size = sock->file_size;
        msg = deliver(0, sock_fd, filepath1, file_size);

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
        rcv_request(sock); // a. receive file size
        file_size = sock->file_size;

        msg = receive(0, sock_fd, filepath2, file_size);
        
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
        fprintf(stderr, "Client: command is NULL\n");
    } else {
        set_command(sock, str_to_cmd_enum(argv[1]));
    }

    if (sock->command == STOP) {
        return;
    }
    if (argv[2] == NULL) {
        fprintf(stderr, "Client: filepath1 is NULL\n");
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
    } else {
        if (sock->first_filename != NULL) {
            char* filename1 = strdup(sock->first_filename);
            set_sec_fileInfo(filename1, sock);
            free(filename1);
        } else {
            fprintf(stderr, "Client: filepath2 and filename1 is NULL\n");
        }
    }
    
    set_sec_filepath(sock);
}

/*
__print_server_resp
*/
void __print_server_resp(const char* response) {
  printf("Client:\n Server Response: %s\n", response);
}