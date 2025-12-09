/**
 * @file socket.c / source code for Socket.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "socket_md.h"

/*
create_socket_md
*/
socket_md_t* create_socket_md(int client_fd) {
    socket_md_t* sock = (socket_md_t*)calloc(1, sizeof(socket_md_t));
    if (sock == NULL) {
        fprintf(stderr, "ERROR: dynamic memory was not able to be allocated\n");
        return NULL;
    }

    sock->client_sock_fd = client_fd;
    sock->file_size = 0;
    sock->command = NULL_VAL;
    sock->first_dirs = NULL;
    sock->sec_dirs = NULL;
    sock->first_filename = NULL;
    sock->sec_filename = NULL;
    sock->first_filepath = NULL;
    sock->sec_filepath = NULL;
    sock->first_file_ext = NULL;
    sock->sec_file_ext = NULL;

    return sock;
}

/*
str_to_cmd_enum
*/
commands str_to_cmd_enum(const char* str) {
    if (strcmp(str, "WRITE") == 0) return WRITE;
    if (strcmp(str, "GET") == 0) return GET;
    if (strcmp(str, "RM") == 0) return RM;
    if (strcmp(str, "STOP") == 0) return STOP;
    return NULL_VAL;
}

/*
cmd_enum_to_str
*/
const char* cmd_enum_to_str(commands cmd) {
    if (cmd == WRITE) return "WRITE";
    if (cmd == GET) return "GET";
    if (cmd == RM) return "RM";
    if (cmd == STOP) return "STOP";
    return NULL;
}

/*
set_command
*/
void set_command(socket_md_t* sock, commands command) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }

    sock->command = command;
    return;
}

/*
print_sock_metada
*/
void print_sock_metada(socket_md_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }
    printf("Command: %s\n", cmd_enum_to_str(sock->command));
    printf("First Filepath: %s, First Dirs: %s, First Filename: %s, First File Ext: %s\n", sock->first_filepath, sock->first_dirs,
        sock->first_filename, sock->first_file_ext);
    printf("Sec Filepath: %s, Sec Dirs: %s, Sec Filename: %s, Sec File Ext: %s\n", sock->sec_filepath, sock->sec_dirs,
        sock->sec_filename, sock->sec_file_ext);
    return;
}

/*
Notes:
- check if there's a folder if not then use default path and file
- need to check if the sock->first_dirs is null - if pdir is null then just return filename
*/
void set_first_filepath(socket_md_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }

    char* filename1 = sock->first_filename;
    char* dirs1 = sock->first_dirs;
    int file_path_len = 0;

    if (dirs1 != NULL) {
        if(filename1 != NULL) {
            file_path_len = strlen(dirs1) + strlen(filename1) + 1;
            char path[file_path_len]; // ex: data/file.txt
            sprintf(path, "%s%s", dirs1, filename1);
            sock->first_filepath = strdup(path);
        } else {
            file_path_len = strlen(dirs1) + 1;
            char path[file_path_len]; // ex: data/file.txt
            sprintf(path, "%s", dirs1);
            sock->first_filepath = strdup(path);
        }
    } else {
        if(filename1 != NULL) {
            file_path_len = strlen(filename1) + 1;
            char path[file_path_len];
            sprintf(path, "%s", filename1);
            sock->first_filepath = strdup(path);
        }
    }
    return;
}

// WRITE - if argv[3] is null then use file name of arfv[2] / GET - if argv[3] is null then need to use default local path
void set_sec_filepath(socket_md_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }
    
    commands cmd = sock->command;
    char* filename2 = sock->sec_filename;
    char* filename1 = sock->first_filename;
    char* dirs2 = sock->sec_dirs;
    int file_path_len = 0;

    if (dirs2 == NULL && cmd == WRITE) {
            file_path_len = strlen(DEFAULT_SERVER_DIR) + strlen(filename2) + 1;
            char path[file_path_len]; // ex: data/file.txt
            sprintf(path, "%s%s", DEFAULT_SERVER_DIR, filename2);
            sock->sec_filepath = strdup(path);
    } else if (dirs2 == NULL && cmd == GET) { // if local folder or file is omitted then use current folder
            file_path_len = strlen(DEFAULT_CLIENT_DIR) + strlen(filename2) + 1;
            char path[file_path_len]; // ex: data/file.txt
            sprintf(path, "%s%s", DEFAULT_CLIENT_DIR, filename2);
            sock->sec_filepath = strdup(path);
    } else if (filename2 == NULL) {
        file_path_len = strlen(dirs2) + strlen(filename1) + 1;
        char path[file_path_len]; // ex: data/file.txt
        sprintf(path, "%s%s", dirs2, filename1);
        sock->sec_filepath = strdup(path);
    } else {
        file_path_len = strlen(dirs2) + strlen(filename2) + 1;
        char path[file_path_len]; // ex: data/file.txt
        sprintf(path, "%s%s", dirs2, filename2);
        sock->sec_filepath = strdup(path);
    }
    

    return;
}

/*
Assumptions:
- if there is no slash and no file extension then it will be treated as a file
- if there is a slash but no file extension then it will be treated as a folder
- accounts for WRITE, GET, and RM
*/
void set_first_fileInfo(const char *path, socket_md_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }

    // if first file path info is missing then need to exit since first path cannot be missing
    if (path == NULL) {
        fprintf(stderr, "ERROR: first path from arv[2] is NULL\n");
        return;
    }
    // Find the last occurrence of the directory separator
    const char *last_slash = strrchr(path, LITERAL_PATH_DELIMITER);
    const char *last_dot = strrchr(path, '.');  // Find the last period in the filename
    if (last_slash != NULL) {
        // Separate out the extension from the filename
        if (last_dot != NULL) {
            // Copy the directory part
            size_t dir_len = last_slash - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
            sock->first_dirs = (char*)calloc(dir_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
            if (sock->first_dirs == NULL) {
                // Handle memory allocation failure if needed
                fprintf(stderr, "calloc failed for first_dirs\n");
                return;
            }
            strncpy(sock->first_dirs, path, dir_len);  // Copy the directory part into first_dirs
            sock->first_dirs[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

            // If a period is found, extract the extension
            sock->first_file_ext = strdup(last_dot + 1);  // Copy the extension into first_file_ext (without the period)
            
            // Copy the filename part (after the last separator)
            sock->first_filename = strdup(last_slash + 1);  // Copy the filename part into first_filename
        } else {
            sock->first_dirs = strdup(path); // Copy the whole path into first_filename
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
            if (sock->command != RM) {
                sock->first_file_ext = strdup(DEFAULT_FILE_EXT);  // Copy the extension into first_file_ext (without the period)
                // If no directory separator is found, the whole path is the filename
                size_t fn_len = strlen(path) + 1 + strlen(DEFAULT_FILE_EXT); // +1 for the missing period
                sock->first_filename = (char*)calloc(fn_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
                sprintf(sock->first_filename, "%s.%s", path, sock->first_file_ext); // Copy the whole path into first_filename
            }
        }
        return;
    }
    return;
}


void set_sec_fileInfo(const char *path, socket_md_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return;
    }
    // if sec file path info is missing from command then make from first file path info
    if (path == NULL) { // will handle this when sending args are sent to server
        fprintf(stderr, "second path from arv[3] is NULL\n");
        return;
    }
    // Find the last occurrence of the directory separator
    const char *last_slash = strrchr(path, LITERAL_PATH_DELIMITER);
    const char *last_dot = strrchr(path, '.');  // Find the last period in the filename
    if (last_slash != NULL) {
        // Separate out the extension from the filename
        if (last_dot != NULL) {
            // Copy the directory part
            size_t dir_len = last_slash - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
            sock->sec_dirs = (char*)calloc(dir_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
            if (sock->sec_dirs == NULL) {
                // Handle memory allocation failure if needed
                fprintf(stderr, "calloc failed for first_dirs\n");
                return;
            }
            strncpy(sock->sec_dirs, path, dir_len);  // Copy the directory part into sec_dirs
            sock->sec_dirs[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

            // If a period is found, extract the extension
            sock->sec_file_ext = strdup(last_dot + 1);  // Copy the extension into sec_file_ext (without the period)
            
            // Copy the filename part (after the last separator)
            sock->sec_filename = strdup(last_slash + 1);  // Copy the filename part into sec_filename
        } else {
            sock->sec_dirs = strdup(path); // Copy the whole path into sec_filename
        }
        return;

    } else { // if there's no directory just the file
        // Separate out the extension from the filename
        if (last_dot != NULL) {
            // If a period is found, extract the extension
            sock->sec_file_ext = strdup(last_dot + 1);  // Copy the extension into sec_file_ext (without the period)
            // If no directory separator is found, the whole path is the filename
            sock->sec_filename = strdup(path); // Copy the whole path into sec_filename
        } else { // if no extension then not a valid filename so will use first_filename but if first_filename is not valid either then will construct a defailt filename using the string that is neither a dir or a filename
            if(sock->first_file_ext != NULL) {
                sock->sec_filename = strdup(sock->first_filename);
            } else {
                sock->sec_file_ext = strdup(DEFAULT_FILE_EXT);  // Copy the extension into sec_file_ext (without the period)
                // If no directory separator is found, the whole path is the filename
                size_t fn_len = strlen(path) + 1 + strlen(DEFAULT_FILE_EXT); // +1 for the missing period
                sock->sec_filename = (char*)calloc(fn_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
                sprintf(sock->sec_filename, "%s.%s", path, sock->sec_file_ext); // Copy the whole path into sec_filename
            }
        }
        return;
    }
    return;
}


void free_socket(socket_md_t* sock) {
    if (sock != NULL) {
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
            shutdown(sock->client_sock_fd, SHUT_RDWR); // disables both sends and receives
            close(sock->client_sock_fd);
            printf("Closed client socket fd: %d\n", sock->client_sock_fd);
        }

        free(sock);
    }
}