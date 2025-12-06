/**
 * @file socket.c / source code for Socket.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Nov 11, 2025 / Fall 2025
 *
*/

#include "socket.h"

socket_t* create_socket() {
    socket_t* sock = (socket_t*) calloc(1, sizeof(socket_t));
    if (sock == NULL) {
        fprintf(stderr, "ERROR: dynamic memory was not able to be allocated");
        exit(1);
    }

    sock->command = NULL_VAL;
    sock->read_pdir = NULL;
    sock->write_pdir = NULL;
    sock->read_filename = NULL;
    sock->write_filename = NULL;
    sock->read_filepath = NULL;
    sock->write_filepath = NULL;

    return sock;
}


commands str_to_cmd_enum(const char* str) {
    if (strcmp(str, "WRITE") == 0) return WRITE;
    if (strcmp(str, "GET") == 0) return GET;
    if (strcmp(str, "RM") == 0) return RM;
    if (strcmp(str, "STOP") == 0) return STOP;
    return NULL_VAL;
}


const char* cmd_enum_to_str(commands cmd) {
    if (cmd == WRITE) return "WRITE";
    if (cmd == GET) return "GET";
    if (cmd == RM) return "RM";
    if (cmd == STOP) return "STOP";
    return NULL;
}


void set_sock_command(socket_t* sock, commands command) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }

    sock->command = command;
}


void print_read_file_info(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        return;
    }
    printf("Parent Dir: %s, Filename: %s\n", sock->read_pdir, sock->read_filename);
}

/*
Notes:
- check if there's a folder if not then use default path and file
- need to check if the sock->read_pdir is null - if pdir is null then just return filename
*/
void set_sock_read_filepath(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }

    int file_path_len = 0;
    if (sock->read_pdir != NULL) {
        file_path_len = strlen(DEFAULT_CLIENT_PATH) + strlen(sock->read_pdir) + strlen(sock->read_filename) + 1;
        char path[file_path_len]; // ex: data/file.txt
        sprintf(path, "%s%s%s", DEFAULT_CLIENT_PATH, sock->read_pdir, sock->read_filename);
        sock->read_filepath = strdup(path);
    } else {
        file_path_len = strlen(DEFAULT_CLIENT_PATH) + strlen(sock->read_filename) + 1;
        char path[file_path_len];
        sprintf(path, "%s%s", DEFAULT_CLIENT_PATH, sock->read_filename);
        sock->read_filepath = strdup(path);
    }
}


void set_sock_write_filepath(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }

    int file_path_len = 0;
    switch(sock->command) {
        case WRITE:
            if (sock->write_filename != NULL) {
                file_path_len = strlen(DEFAULT_SERVER_PATH) + strlen(sock->write_filename) + 1;
                char path[file_path_len]; // ex: data/file.txt
                sprintf(path, "%s%s", DEFAULT_SERVER_PATH, sock->write_filename);
                sock->write_filepath = strdup(path);
            } else {
                file_path_len = strlen(DEFAULT_SERVER_PATH) + strlen(sock->read_filename) + 1;
                char path[file_path_len]; // ex: data/file.txt
                sprintf(path, "%s%s", DEFAULT_SERVER_PATH, sock->read_filename);
                sock->write_filepath = strdup(path);
            }
            break;
        case GET:
            if (sock->write_filename != NULL) {
                file_path_len = strlen(DEFAULT_CLIENT_PATH) + strlen(sock->write_filename) + 1;
                char path[file_path_len];
                sprintf(path, "%s%s", DEFAULT_CLIENT_PATH, sock->write_filename);
                sock->write_filepath = strdup(path);
            } else {
                file_path_len = strlen(DEFAULT_CLIENT_PATH) + strlen(sock->read_filename) + 1;
                char path[file_path_len];
                sprintf(path, "%s%s", DEFAULT_CLIENT_PATH, sock->read_filename);
                sock->write_filepath = strdup(path);
            }
            break;
        default:
            break;
    }
}


void free_socket(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }

    if (sock->read_pdir != NULL) {
        free(sock->read_pdir);
    }
    if (sock->write_pdir != NULL) {
        free(sock->write_pdir);
    }

    if (sock->read_filename != NULL) {
        free(sock->read_filename);
    }
    if (sock->write_filename != NULL) {
        free(sock->write_filename);
    }

    if (sock->read_filepath != NULL) {
        free(sock->read_filepath);
    }
    if (sock->write_filepath != NULL) {
        free(sock->write_filepath);
    }

    if (sock->client_sock_fd >= 0) { // if valid file descriptor is assigned then close it
        close(sock->client_sock_fd);
    }
    if (sock->server_sock_fd >= 0) {
        close(sock->server_sock_fd);
    }

    free(sock);
}

void send_file(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }
    if (sock->read_filepath == NULL) {
        fprintf(stderr, "ERROR: read filename is NULL");
        exit(1);

    }
    if (sock->server_sock_fd < 0) {
        fprintf(stderr, "ERROR: server socket file descriptor is invalid");
        exit(1);
    }
    if (sock->client_sock_fd < 0) {
        fprintf(stderr, "ERROR: client socket file descriptor is invalid");
        exit(1);
    }

    int send_socket;
    switch (sock->command) {
    case WRITE:
        /* code */
         send_socket = sock->client_sock_fd;
        break;
    case GET:
        send_socket = sock->server_sock_fd;
        break;
    default:
        break;
    }

    // printf("Local File path: %s\n", file_path);
    FILE *file = fopen(sock->read_filepath, "rb"); // "rb" for read binary
    if (file == NULL) {
        perror("Error opening read file");
        free_socket(sock);
        exit(1);
    }

    // Get the size of the file
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error seeking to end of read file");
        free_socket(sock);
        fclose(file);
        exit(1);
    }
    long file_size = ftell(file);
    if(file_size < 0) {
        perror("Error getting read file size");
        free_socket(sock);
        fclose(file);
        exit(1);

    }
    if (fseek(file, 0, SEEK_SET) != 0) { // reset the file pointer to the beginning
        perror("Error seeking to start of read file");
        free_socket(sock);
        fclose(file);
        exit(1);
    }
    
    // send the file size
    uint32_t size = htonl(file_size);
    send(send_socket, &size, sizeof(size), 0);

    // send the file data
    char buffer[CHUNK_SIZE]; // buffer to hold file chunks
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) { // reads the given amount of data (CHUNK_SIZE) from the file into the buffer
      size_t total_sent = 0;

      while (total_sent < bytes_read) {
          ssize_t sent = send(send_socket, buffer + total_sent,
                              bytes_read - total_sent, 0);

          if (sent < 0) {
              perror("Unable to send message\n");
              // handle error (disconnect, etc.)
              exit(1);
          }

          total_sent += sent;
        }
    }
    
    fclose(file);
    return;
}


void rcv_file(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }
    if (sock->write_filepath == NULL) {
        fprintf(stderr, "ERROR: write filename is NULL");
        exit(1);

    }
    if (sock->server_sock_fd < 0) {
        fprintf(stderr, "ERROR: server socket file descriptor is invalid");
        exit(1);
    }
    if (sock->client_sock_fd < 0) {
        fprintf(stderr, "ERROR: client socket file descriptor is invalid");
        exit(1);
    }

    int rcv_socket;
    switch (sock->command) {
    case WRITE:
        /* code */
        rcv_socket = sock->server_sock_fd;
        break;
    case GET:
        rcv_socket = sock->client_sock_fd;
        break;
    default:
        break;
    }
    uint32_t size;
    recv(rcv_socket, &size, sizeof(size), 0);
    size = ntohl(size);
    printf("File size: %u\n", size);

    char buffer[CHUNK_SIZE];
    ssize_t received;

    FILE *out_file = fopen(sock->write_filepath, "wb");
    if (out_file == NULL) {
        perror("Error opening write file");
        free_socket(sock);
        exit(1);
    }

    int chunk_count = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;
    for (int i = 0; i < chunk_count; i++) {
        while ((received = recv(rcv_socket, buffer, CHUNK_SIZE, 0)) > 0) {
            fwrite(buffer, 1, received, out_file);
        }
    }

    fclose(out_file);
}
