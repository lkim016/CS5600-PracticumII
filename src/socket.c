/**
 * @file socket.c / source code for Socket.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Nov 11, 2025 / Fall 2025
 *
 */

#include "socket.h"

/**
 * @brief Create and initialize a server socket object on the heap.
 *
 * @return socket_t* - return the initialized cache object
 */
socket_t* create_socket() {
    socket_t* sock = (socket_t*) calloc(1, sizeof(socket_t));
    if (sock == NULL) {
        fprintf(stderr, "ERROR: dynamic memory was not able to be allocated");
        exit(1);
    }
    sock->command = NULL_VAL;
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

const char *cmd_enum_to_str(commands cmd) {
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

void split_read_path(const char *path, socket_t* sock) {
    // Find the last occurrence of the directory separator
    const char *last_sep = strrchr(path, PATH_DELIMITER);
    
    if (last_sep != NULL) {
        // Copy the directory part
        size_t dir_len = last_sep - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
        sock->read_pdir = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->read_pdir == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for read_pdir");
            exit(1);
        }
        strncpy(sock->read_pdir, path, dir_len);  // Copy the directory part into read_pdir
        // sock->read_pdir[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Copy the filename part (after the last separator)
        sock->read_filename = (char*)calloc(strlen(last_sep + 1) + 1, sizeof(char));  // Allocate memory for the filename part
        if (sock->read_filename == NULL) {
            perror("calloc failed for read_filename");
            exit(1);
        }
        strcpy(sock->read_filename, last_sep + 1);  // Copy the filename part into read_filename

        /*
        // Separate out the extension from the filename
        char *ext = strrchr(sock->read_filename, '.');  // Find the last period in the filename
        if (ext != NULL) {
            // If a period is found, extract the extension
            sock->read_extension = strdup(ext + 1);  // Copy the extension into read_extension (without the period)
        } else {
            // If no period is found, there is no extension
            sock->read_extension = NULL;
        }
        */
    } else {
        // If no directory separator is found, the whole path is the filename
        size_t dir_len = strlen(path);  // Directory length will be the full length of the path
        sock->read_pdir = (char*)calloc(1, sizeof(char));  // Allocate memory for an empty string (no directory part)
        if (sock->read_pdir == NULL) {
            perror("calloc failed for read_pdir (no separator)");
            exit(1);
        }
        sock->read_filename = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the full filename
        if (sock->read_filename == NULL) {
            perror("calloc failed for read_filename (no separator)");
            exit(1);
        }
        strcpy(sock->read_filename, path);  // Copy the whole path into read_filename
        sock->read_pdir[0] = '\0';  // No directory part, set it as an empty string

        /*
        // Separate out the extension from the filename
        char *ext = strrchr(sock->read_filename, '.');  // Find the last period in the filename
        if (ext != NULL) {
            // If a period is found, extract the extension
            sock->read_extension = strdup(ext + 1);  // Copy the extension into read_extension (without the period)
        } else {
            // If no period is found, there is no extension
            sock->read_extension = NULL;
        }
        */
    }
}

void print_read_file_info(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        return;
    }
    printf("Parent Dir: %s, Filename: %s", sock->read_pdir, sock->read_filename);
}

void split_write_path(const char *path, socket_t* sock) {
    // Find the last occurrence of the directory separator
    const char *last_sep = strrchr(path, PATH_DELIMITER);
    
    if (last_sep != NULL) {
        // Copy the directory part
        size_t dir_len = last_sep - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
        sock->write_pdir = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->write_pdir == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for write_pdir");
            exit(1);
        }
        strncpy(sock->write_pdir, path, dir_len);  // Copy the directory part into write_pdir
        // sock->write_pdir[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Copy the filename part (after the last separator)
        sock->write_filename = (char*)calloc(strlen(last_sep + 1) + 1, sizeof(char));  // Allocate memory for the filename part
        if (sock->write_filename == NULL) {
            perror("calloc failed for write_filename");
            exit(1);
        }
        strcpy(sock->write_filename, last_sep + 1);  // Copy the filename part into write_filename

    } else {
        // If no directory separator is found, the whole path is the filename
        size_t dir_len = strlen(path);  // Directory length will be the full length of the path
        sock->write_pdir = (char*)calloc(1, sizeof(char));  // Allocate memory for an empty string (no directory part)
        if (sock->write_pdir == NULL) {
            perror("calloc failed for write_pdir (no separator)");
            exit(1);
        }
        sock->write_filename = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the full filename
        if (sock->write_filename == NULL) {
            perror("calloc failed for write_filename (no separator)");
            exit(1);
        }
        strcpy(sock->write_filename, path);  // Copy the whole path into write_filename
        sock->write_pdir[0] = '\0';  // No directory part, set it as an empty string

    }
}
/*
const char *get_default_read_path(socket_t* sock) {
    switch(sock->command) {
        case WRITE:
            int file_path_len = strlen(DEFAULT_CLIENT_PATH) + strlen(DEFAULT_CLIENT_FILENAME) + 1;
            char result[file_path_len]; // ex: data/file.txt
            sprintf(result, "%s%s", DEFAULT_CLIENT_PATH, DEFAULT_CLIENT_FILENAME);
        case GET:
            int file_path_len = strlen(DEFAULT_SERVER_PATH) + strlen(DEFAULT_SERVER_FILENAME) + 1;
            char result[file_path_len]; // ex: data/file.txt
            sprintf(result, "%s%s", DEFAULT_SERVER_PATH, DEFAULT_SERVER_FILENAME);
            sock->read_filepath = strdup(result);
            return result;
        default:
            return NULL;
    }
}

void set_sock_read_filepath(socket_t* sock, const char* read_filepath) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }
    // check if there's a folder if not then use default path and file
    if (read_filepath == NULL) {
        const char* default_read_path = get_default_read_path(sock);
        sock->read_filepath = strdup(default_read_path);
    } else {
        // check if there's an extension
        sock->read_filepath = strdup(read_filepath);
    }
}

const char *get_default_write_path(socket_t* sock) {
    switch(sock->command) {
        case WRITE:
            int file_path_len = strlen(DEFAULT_SERVER_PATH) + strlen(DEFAULT_SERVER_FILENAME) + 1;
            char result[file_path_len]; // ex: data/file.txt
            sprintf(result, "%s%s", DEFAULT_SERVER_PATH, DEFAULT_SERVER_FILENAME);
            return result;
        case GET:
            int file_path_len = strlen(DEFAULT_CLIENT_PATH) + strlen(DEFAULT_CLIENT_FILENAME) + 1;
            char result[file_path_len]; // ex: data/file.txt
            sprintf(result, "%s%s", DEFAULT_CLIENT_PATH, DEFAULT_CLIENT_FILENAME);
            return result;
        default:
            return NULL;
    }
}

void set_sock_write_filepath(socket_t* sock, const char* write_filepath) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }

    // check if there's a folder if not then use default path and file
    if (write_filepath == NULL) {
        const char* default_write_path = get_default_read_path(sock);
        sock->write_filepath = strdup(default_write_path);
    } else {
        sock->write_filepath = strdup(write_filepath);
    }
}
*/

/**
 * @brief free the socket object and its members.
 *
 * @param socket socket_t* - the pointer to the socket object
 */
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


// socket_send_write() {}


/*
ssize_t send_all(int sock, const void *buf, size_t len) {
    size_t sent = 0;
    const char *p = buf;
    while (sent < len) {
        ssize_t n = send(sock, p + sent, len - sent, 0);
        if (n <= 0) return n;
        sent += n;
    }
    return sent;
}


ssize_t rcv_all(int sock, void *buf, size_t len) {
    size_t recvd = 0;
    char *p = buf;
    while (recvd < len) {
        ssize_t n = recv(sock, p + recvd, len - recvd, 0);
        if (n <= 0) return n;
        recvd += n;
    }
    return recvd;
}

// SENDER
int send_msg(int sockfd, const char* command, const char* filename, const uint8_t* file_data, size_t file_size) {
    msg_header_t header;

    header.command_len = htonl(strlen(command));
    header.filename_len = htonl(strlen(filename));
    header.file_size = htonll(file_size);
    
    // Send header
    if (send_all(sockfd, &header, sizeof(header)) < 0)
        return -1;
    
    // Send command
    if (send_all(sockfd, command, strlen(command)) < 0)
        return -1;
    
    // Send filename
    if (send_all(sockfd, filename, strlen(filename)) < 0)
        return -1;
    
    // Send file data
    if (send_all(sockfd, file_data, file_size) < 0)
        return -1;
    
    return 0;
}

// RECEIVER
int rcv_msg(int sockfd, char** command, char** filename, uint8_t** file_data, size_t* file_size) {
    msg_header_t header;
    
    // Receive header
    if (rcv_all(sockfd, &header, sizeof(header)) < 0)
        return -1;
    
    // Convert from network byte order
    uint32_t cmd_len = ntohl(header.command_len);
    uint32_t fname_len = ntohl(header.filename_len);
    *file_size = ntohll(header.file_size);
    
    // Validate lengths (security!)
    if (cmd_len > 1024 || fname_len > 4096 || *file_size > 1ULL << 32) {
        return -1;  // Reject suspiciously large values
    }
    
    // Allocate and receive command
    *command = malloc(cmd_len + 1);
    if (recv_all(sockfd, *command, cmd_len) < 0) {
        free(*command);
        return -1;
    }
    (*command)[cmd_len] = '\0';
    
    // Allocate and receive filename
    *filename = malloc(fname_len + 1);
    if (recv_all(sockfd, *filename, fname_len) < 0) {
        free(*command);
        free(*filename);
        return -1;
    }
    (*filename)[fname_len] = '\0';
    
    // Allocate and receive file data
    *file_data = malloc(*file_size);
    if (recv_all(sockfd, *file_data, *file_size) < 0) {
        free(*command);
        free(*filename);
        free(*file_data);
        return -1;
    }
    
    return 0;
}
*/