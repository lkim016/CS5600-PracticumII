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

// WRITE - if argv[3] is null then use file name of arfv[2] / GET - if argv[3] is null then need to use default local path
void set_sock_sec_filepath(socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }

    int file_path_len = 0;
    switch(sock->command) {
        case WRITE:
            if (sock->sec_dirs == NULL) {
                file_path_len = strlen(DEFAULT_SERVER_DIR) + strlen(sock->sec_filename) + 1;
                char path[file_path_len]; // ex: data/file.txt
                sprintf(path, "%s%s", DEFAULT_SERVER_DIR, sock->sec_filename);
                sock->sec_filepath = strdup(path);
            } else {
                file_path_len = strlen(sock->sec_dirs) + strlen(sock->sec_filename) + 1;
                char path[file_path_len]; // ex: data/file.txt
                sprintf(path, "%s%s", sock->sec_dirs, sock->sec_filename);
                sock->sec_filepath = strdup(path);
            }
            
            break;
        case GET: // if local folder or file is omitted then use current folder
            if (sock->sec_dirs == NULL) {
                file_path_len = strlen(DEFAULT_CLIENT_DIR) + strlen(sock->sec_filename) + 1;
                char path[file_path_len]; // ex: data/file.txt
                sprintf(path, "%s%s", DEFAULT_CLIENT_DIR, sock->sec_filename);
                sock->sec_filepath = strdup(path);
            } else {
                file_path_len = strlen(sock->sec_dirs) + strlen(sock->sec_filename) + 1;
                char path[file_path_len]; // ex: data/file.txt
                sprintf(path, "%s%s", sock->sec_dirs, sock->sec_filename);
                sock->sec_filepath = strdup(path);
            }
            break;
        default:
            break;
    }


    return;
}


void set_first_file_ext(socket_t* sock) {
    
    return;
}


void set_sec_file_ext(socket_t* sock) {
    // Separate out the extension from the filename
    const char *last_dot = strrchr(sock->sec_filename, '.');  // Find the last period in the filename
    if (last_dot != NULL) {
        // If a period is found, extract the extension
        sock->sec_file_ext = strdup(last_dot + 1);  // Copy the extension into sec_file_ext (without the period)
        return;
    }
    return;
}

/*
Assumptions:
- if there is no slash and no file extension then it will be treated as a file
- if there is a slash but no file extension then it will be treated as a folder
*/
void set_first_fileInfo(const char *path, socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        exit(1);
    }
    // Find the last occurrence of the directory separator
    const char *last_slash = strrchr(path, SINGLE_PATH_DELIMITER);
    const char *last_dot = strrchr(path, '.');  // Find the last period in the filename
    if (last_slash != NULL) {
        // Copy the directory part
        size_t dir_len = last_slash - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
        sock->first_dirs = (char*)calloc(dir_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
        if (sock->first_dirs == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for first_dirs\n");
            exit(1);
        }
        strncpy(sock->first_dirs, path, dir_len);  // Copy the directory part into first_dirs
        // sock->first_dirs[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Separate out the extension from the filename
        if (last_dot != NULL) {
            // If a period is found, extract the extension
            sock->first_file_ext = strdup(last_dot + 1);  // Copy the extension into first_file_ext (without the period)
            
            // Copy the filename part (after the last separator)
            sock->first_filename = strdup(last_slash + 1);  // Copy the filename part into first_filename
        }
        return;

    } else { // if there's no directory just the file
        // Separate out the extension from the filename
        if (last_dot != NULL) {
            // If a period is found, extract the extension
            sock->first_file_ext = strdup(last_dot + 1);  // Copy the extension into first_file_ext (without the period)
            // If no directory separator is found, the whole path is the filename
            sock->first_filename = strdup(path); // Copy the whole path into first_filename
        } else {
            sock->first_file_ext = strdup(DEFAULT_FILE_EXT);  // Copy the extension into first_file_ext (without the period)
            // If no directory separator is found, the whole path is the filename
            size_t fn_len = strlen(path) + 1 + strlen(DEFAULT_FILE_EXT); // +1 for the missing period
            sock->first_filename = (char*)calloc(fn_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
            sprintf(sock->first_filename, "%s.%s", path, sock->first_file_ext); // Copy the whole path into first_filename
        }
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
    if (last_slash != NULL) {
        // Copy the directory part
        size_t dir_len = last_slash - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
        sock->sec_dirs = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->sec_dirs == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for sec_dirs\n");
            exit(1);
        }
        strncpy(sock->sec_dirs, path, dir_len);  // Copy the directory part into sec_dirs
        // sock->sec_dirs[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Copy the filename part (after the last separator)
        sock->sec_filename = strdup(last_slash + 1);  // Copy the filename part into sec_filename
        return;

    } else {
        // If no directory separator is found, the whole path is the filename
        sock->sec_filename = strdup(path);  // Copy the whole path into sec_filename
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