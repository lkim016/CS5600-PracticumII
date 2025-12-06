/**
 * @file socket.c / source code for Socket.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "socket.h"

socket_t* create_socket() {
    socket_t* sock = (socket_t*) calloc(1, sizeof(socket_t));
    if (sock == NULL) {
        fprintf(stderr, "ERROR: dynamic memory was not able to be allocated\n");
        exit(1);
    }

    sock->command = NULL_VAL;
    sock->read_dirs = NULL;
    sock->write_dirs = NULL;
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
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }

    sock->command = command;
}


void print_read_file_info(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }
    printf("Parent Dir: %s, Filename: %s, File Ext: %s\n", sock->read_dirs, sock->read_filename, sock->read_file_ext);
}

void print_write_file_info(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }
    printf("Parent Dir: %s, Filename: %s, File Ext: %s\n", sock->write_dirs, sock->write_filename, sock->write_file_ext);
}

/*
Notes:
- check if there's a folder if not then use default path and file
- need to check if the sock->read_dirs is null - if pdir is null then just return filename
*/
void set_sock_read_filepath(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }

    int file_path_len = 0;
    if (sock->read_dirs != NULL) {
        file_path_len = strlen(sock->read_dirs) + strlen(sock->read_filename) + 1;
        char path[file_path_len]; // ex: data/file.txt
        sprintf(path, "%s%s", sock->read_dirs, sock->read_filename);
        sock->read_filepath = strdup(path);
    } else {
        file_path_len = strlen(sock->read_filename) + 1;
        char path[file_path_len];
        sprintf(path, "%s", sock->read_filename);
        sock->read_filepath = strdup(path);
    }
}


void set_sock_write_filepath(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }

    int file_path_len = 0;
    switch(sock->command) {
        case WRITE:
                if (sock->write_dirs != NULL) {
                    file_path_len = strlen(sock->write_dirs) + strlen(sock->write_filename) + 1;
                    char path[file_path_len]; // ex: data/file.txt
                    sprintf(path, "%s%s", sock->write_dirs, sock->write_filename);
                    sock->write_filepath = strdup(path);
                } else {
                    file_path_len = strlen(sock->write_filename) + 1;
                    char path[file_path_len]; // ex: data/file.txt
                    sprintf(path, "%s", sock->write_filename);
                    sock->write_filepath = strdup(path);
                }
            break;
        case GET: // if local folder or file is omitted then use current folder
                if (sock->write_dirs != NULL) {
                    file_path_len = strlen(sock->write_dirs) + strlen(sock->write_filename) + 1;
                    char path[file_path_len]; // ex: data/file.txt
                    sprintf(path, "%s%s", sock->write_dirs, sock->write_filename);
                    sock->write_filepath = strdup(path);
                } else {
                    file_path_len = strlen(sock->write_filename) + 1;
                    char path[file_path_len]; // ex: data/file.txt
                    sprintf(path, "%s", sock->write_filename);
                    sock->write_filepath = strdup(path);
                }
            break;
        default:
            break;
    }
}


void set_read_file_ext(socket_t* sock) {
    // Separate out the extension from the filename
    size_t ext_len = 0;
    const char *last_dot = last_dot = strrchr(sock->read_filename, '.');  // Find the last period in the filename
    if (last_dot != NULL) {
        ext_len = last_dot - sock->read_filename + 1;  // pointer arithmetic calculating the length of the file ext part of the filename
        sock->read_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->read_file_ext == NULL) {
            perror("calloc failed for read_file_ext\n");
            exit(1);
        }
        // If a period is found, extract the extension
        sock->read_file_ext = strdup(last_dot + 1);  // Copy the extension into read_file_ext (without the period)
        return;
    }
}


void set_write_file_ext(socket_t* sock) {
    // Separate out the extension from the filename
    size_t ext_len = 0;
    const char *last_dot = last_dot = strrchr(sock->write_filename, '.');  // Find the last period in the filename
    if (last_dot != NULL) {
        ext_len = last_dot - sock->write_filename + 1;  // pointer arithmetic calculating the length of the file ext part of the filename
        sock->write_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->write_file_ext == NULL) {
            perror("calloc failed for write_file_ext\n");
            exit(1);
        }
        // If a period is found, extract the extension
        sock->write_file_ext = strdup(last_dot + 1);  // Copy the extension into write_file_ext (without the period)
        return;
    }
}


void set_read_fileInfo(const char *path, socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }
    // Find the last occurrence of the directory separator
    const char *last_slash = strrchr(path, SINGLE_PATH_DELIMITER);
    size_t dir_len = 0;
    if (last_slash != NULL) {
        // Copy the directory part
        dir_len = last_slash - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
        sock->read_dirs = (char*)calloc(dir_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
        if (sock->read_dirs == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for read_dirs\n");
            exit(1);
        }
        strncpy(sock->read_dirs, path, dir_len);  // Copy the directory part into read_dirs
        // sock->read_dirs[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Copy the filename part (after the last separator)
        sock->read_filename = (char*)calloc(strlen(last_slash + 1) + 1, sizeof(char));  // Allocate memory for the filename part
        if (sock->read_filename == NULL) {
            perror("calloc failed for read_filename\n");
            exit(1);
        }
        strcpy(sock->read_filename, last_slash + 1);  // Copy the filename part into read_filename
        return;

    } else {
        // If no directory separator is found, the whole path is the filename
        dir_len = strlen(path);  // Directory length will be the full length of the path
        sock->read_filename = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the full filename
        if (sock->read_filename == NULL) {
            perror("calloc failed for read_filename (no separator)\n");
            exit(1);
        }
        strcpy(sock->read_filename, path);  // Copy the whole path into read_filename
        return;
    }

}


void set_write_fileInfo(const char *path, socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }
    // Find the last occurrence of the directory separator
    const char *last_slash = strrchr(path, SINGLE_PATH_DELIMITER);
    size_t dir_len = 0;
    if (last_slash != NULL) {
        // Copy the directory part
        dir_len = last_slash - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
        sock->write_dirs = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->write_dirs == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for write_dirs\n");
            exit(1);
        }
        strncpy(sock->write_dirs, path, dir_len);  // Copy the directory part into write_dirs
        // sock->write_dirs[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Copy the filename part (after the last separator)
        sock->write_filename = (char*)calloc(strlen(last_slash + 1) + 1, sizeof(char));  // Allocate memory for the filename part
        if (sock->write_filename == NULL) {
            perror("calloc failed for write_filename\n");
            exit(1);
        }
        strcpy(sock->write_filename, last_slash + 1);  // Copy the filename part into write_filename
        return;

    } else {
        // If no directory separator is found, the whole path is the filename
        dir_len = strlen(path);  // Directory length will be the full length of the path
        sock->write_filename = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the full filename
        if (sock->write_filename == NULL) {
            perror("calloc failed for write_filename (no separator)\n");
            exit(1);
        }
        strcpy(sock->write_filename, path);  // Copy the whole path into write_filename
        return;
    }
}


void free_socket(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }

    if (sock->read_dirs != NULL) {
        free(sock->read_dirs);
    }
    if (sock->write_dirs != NULL) {
        free(sock->write_dirs);
    }

    if (sock->read_filename != NULL) {
        free(sock->read_filename);
    }
    if (sock->write_filename != NULL) {
        free(sock->write_filename);
    }

    if (sock->read_file_ext != NULL) {
        free(sock->read_file_ext);
    }
    if (sock->write_file_ext != NULL) {
        free(sock->write_file_ext);
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