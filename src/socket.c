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
    sock->first_dirs = NULL;
    sock->sec_dirs = NULL;
    sock->first_filename = NULL;
    sock->sec_filename = NULL;
    sock->first_filepath = NULL;
    sock->sec_filepath = NULL;

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
    return;
}


void print_read_file_info(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }
    printf("Parent Dir: %s, Filename: %s, File Ext: %s\n", sock->first_dirs, sock->first_filename, sock->first_file_ext);
    return;
}

void print_write_file_info(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }
    printf("Parent Dir: %s, Filename: %s, File Ext: %s\n", sock->sec_dirs, sock->sec_filename, sock->sec_file_ext);
    return;
}

/*
Notes:
- check if there's a folder if not then use default path and file
- need to check if the sock->first_dirs is null - if pdir is null then just return filename
*/
void set_sock_first_filepath(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }

    int file_path_len = 0;
    if (sock->first_dirs != NULL) {
        file_path_len = strlen(sock->first_dirs) + strlen(sock->first_filename) + 1;
        char path[file_path_len]; // ex: data/file.txt
        sprintf(path, "%s%s", sock->first_dirs, sock->first_filename);
        sock->first_filepath = strdup(path);
    } else {
        file_path_len = strlen(sock->first_filename) + 1;
        char path[file_path_len];
        sprintf(path, "%s", sock->first_filename);
        sock->first_filepath = strdup(path);
    }
    return;
}


void set_sock_sec_filepath(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }

    int dir_len = 0;
    switch(sock->command) {
        case WRITE:
            if (sock->sec_dirs == NULL) {
                dir_len = strlen(DEFAULT_SERVER_DIR) + 1;
                char dir[dir_len]; // ex: data/file.txt
                sprintf(dir, "%s", DEFAULT_SERVER_DIR);
                sock->sec_dirs = strdup(dir);
            }
            break;
        case GET: // if local folder or file is omitted then use current folder
            if (sock->sec_dirs == NULL) {
                dir_len = strlen(DEFAULT_CLIENT_DIR) + 1;
                char dir[dir_len]; // ex: data/file.txt
                sprintf(dir, "%s", DEFAULT_CLIENT_DIR);
                sock->sec_dirs = strdup(dir);
            }
            break;
        default:
            break;
    }

    int file_path_len = strlen(sock->sec_dirs) + strlen(sock->sec_filename) + 1;
    char path[file_path_len]; // ex: data/file.txt
    sprintf(path, "%s%s", sock->sec_dirs, sock->sec_filename);
    sock->sec_filepath = strdup(path);

    return;
}


void set_first_file_ext(socket_t* sock) {
    // Separate out the extension from the filename
    size_t ext_len = 0;
    const char *last_dot = last_dot = strrchr(sock->first_filename, '.');  // Find the last period in the filename
    if (last_dot != NULL) {
        ext_len = last_dot - sock->first_filename + 1;  // pointer arithmetic calculating the length of the file ext part of the filename
        sock->first_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->first_file_ext == NULL) {
            perror("calloc failed for first_file_ext\n");
            exit(1);
        }
        // If a period is found, extract the extension
        sock->first_file_ext = strdup(last_dot + 1);  // Copy the extension into first_file_ext (without the period)
        return;
    }
    return;
}


void set_sec_file_ext(socket_t* sock) {
    // Separate out the extension from the filename
    size_t ext_len = 0;
    const char *last_dot = last_dot = strrchr(sock->sec_filename, '.');  // Find the last period in the filename
    if (last_dot != NULL) {
        ext_len = last_dot - sock->sec_filename + 1;  // pointer arithmetic calculating the length of the file ext part of the filename
        sock->sec_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->sec_file_ext == NULL) {
            perror("calloc failed for sec_file_ext\n");
            exit(1);
        }
        // If a period is found, extract the extension
        sock->sec_file_ext = strdup(last_dot + 1);  // Copy the extension into sec_file_ext (without the period)
        return;
    }
    return;
}


void set_first_fileInfo(const char *path, socket_t* sock) {
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
        sock->first_dirs = (char*)calloc(dir_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
        if (sock->first_dirs == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for first_dirs\n");
            exit(1);
        }
        strncpy(sock->first_dirs, path, dir_len);  // Copy the directory part into first_dirs
        // sock->first_dirs[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Copy the filename part (after the last separator)
        sock->first_filename = (char*)calloc(strlen(last_slash + 1) + 1, sizeof(char));  // Allocate memory for the filename part
        if (sock->first_filename == NULL) {
            perror("calloc failed for first_filename\n");
            exit(1);
        }
        strcpy(sock->first_filename, last_slash + 1);  // Copy the filename part into first_filename
        return;

    } else {
        // If no directory separator is found, the whole path is the filename
        dir_len = strlen(path);  // Directory length will be the full length of the path
        sock->first_filename = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the full filename
        if (sock->first_filename == NULL) {
            perror("calloc failed for first_filename (no separator)\n");
            exit(1);
        }
        strcpy(sock->first_filename, path);  // Copy the whole path into first_filename
        return;
    }
    return;
}


void set_sec_fileInfo(const char *path, socket_t* sock) {
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
        sock->sec_dirs = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->sec_dirs == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for sec_dirs\n");
            exit(1);
        }
        strncpy(sock->sec_dirs, path, dir_len);  // Copy the directory part into sec_dirs
        // sock->sec_dirs[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Copy the filename part (after the last separator)
        sock->sec_filename = (char*)calloc(strlen(last_slash + 1) + 1, sizeof(char));  // Allocate memory for the filename part
        if (sock->sec_filename == NULL) {
            perror("calloc failed for sec_filename\n");
            exit(1);
        }
        strcpy(sock->sec_filename, last_slash + 1);  // Copy the filename part into sec_filename
        return;

    } else {
        // If no directory separator is found, the whole path is the filename
        dir_len = strlen(path);  // Directory length will be the full length of the path
        sock->sec_filename = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the full filename
        if (sock->sec_filename == NULL) {
            perror("calloc failed for sec_filename (no separator)\n");
            exit(1);
        }
        strcpy(sock->sec_filename, path);  // Copy the whole path into sec_filename
        return;
    }
    return;
}


void free_socket(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }

    if (sock->first_dirs != NULL) {
        free(sock->first_dirs);
    }
    if (sock->sec_dirs != NULL) {
        free(sock->sec_dirs);
    }

    if (sock->first_filename != NULL) {
        free(sock->first_filename);
    }
    if (sock->sec_filename != NULL) {
        free(sock->sec_filename);
    }

    if (sock->first_file_ext != NULL) {
        free(sock->first_file_ext);
    }
    if (sock->sec_file_ext != NULL) {
        free(sock->sec_file_ext);
    }
    
    if (sock->first_filepath != NULL) {
        free(sock->first_filepath);
    }
    if (sock->sec_filepath != NULL) {
        free(sock->sec_filepath);
    }

    if (sock->client_sock_fd >= 0) { // if valid file descriptor is assigned then close it
        close(sock->client_sock_fd);
    }
    if (sock->server_sock_fd >= 0) {
        close(sock->server_sock_fd);
    }

    free(sock);
}