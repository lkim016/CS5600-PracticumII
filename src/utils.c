/**
 * @file utils.c / source code for program utilities.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "utils.h"

pthread_mutex_t utils_mutex = PTHREAD_MUTEX_INITIALIZER;  // Filesystem operations mutex

// need to free() result
char* dyn_msg(unsigned long id, const char* part1, const char* part2) {
    size_t len = strlen(part1) + strlen(part2) + 3; // 1 for space, 1 for newline, 1 for null terminator
    char* msg_ptr = calloc(len, sizeof(char));
    if (msg_ptr == NULL) {
        perror("Memory allocation failed\n");
        return NULL;
    }
    sprintf(msg_ptr, "Thread ID#%lu - %s %s\n", id, part1, part2);
    return msg_ptr;
}


int folder_not_exists_make(const char* file_path) {
    if (file_path == NULL) {
    fprintf(stderr, "Invalid path!\n");
    return -1;
}

// Find the last occurrence of the directory separator
const char *last_slash = strrchr(file_path, SINGLE_PATH_DELIMITER);
    if (last_slash == NULL) {
        fprintf(stderr, "Invalid path, no directory separator found.\n");
        return -1;
    }

    // Calculate directory path length and allocate memory
    size_t dir_len = last_slash - file_path;
    char *dirs = (char *)malloc(dir_len + 1);
    if (dirs == NULL) {
        perror("malloc failed for dirs");
        return -1;
    }

    // Copy directory path and null-terminate
    memcpy(dirs, file_path, dir_len);
    dirs[dir_len] = '\0';

    // Allocate initial path buffer
    size_t path_capacity = dir_len + 1;
    char *path = (char *)malloc(path_capacity);
    if (path == NULL) {
        perror("malloc failed for path");
        free(dirs);
        return -1;
    }
    path[0] = '\0';

    // Tokenize and create directories
    char *saveptr = NULL;
    char *token = strtok_r(dirs, "/", &saveptr);
    int first_token = 1;

    while (token != NULL) {
        // Calculate required size
        size_t token_len = strlen(token);
        size_t current_len = strlen(path);
        size_t required_size = current_len + token_len + 2; // +1 for '/', +1 for '\0'

        // Reallocate if needed
        if (required_size > path_capacity) {
            path_capacity = required_size;
            char *temp = realloc(path, path_capacity);
            if (temp == NULL) {
                perror("realloc failed for path");
                free(path);
                free(dirs);
                return -1;
            }
            path = temp;
        }

        // Append delimiter if not first token
        if (!first_token) {
            path[current_len++] = SINGLE_PATH_DELIMITER;
            path[current_len] = '\0';
        }
        first_token = 0;

        // Append token
        strcat(path, token);

        // Create directory if it doesn't exist
        struct stat st;
        if (stat(path, &st) != 0) {
            // Directory doesn't exist, create it
            if (mkdir(path, 0755) != 0 && errno != EEXIST) {
                perror("mkdir failed");
                fprintf(stderr, "Failed to create directory: %s\n", path);
                free(path);
                free(dirs);
                return -1;
            }
        } else if (!S_ISDIR(st.st_mode)) {
            // Path exists but is not a directory
            fprintf(stderr, "Path exists but is not a directory: %s\n", path);
            free(path);
            free(dirs);
            return -1;
        }

        token = strtok_r(NULL, "/", &saveptr);
    }

    free(dirs);
    free(path);
    return 0;
}

/**
 * @brief Function to remove a directory and all of its contents for command RM
 *
 * @param path const char* - the socket metadata object's folder path (write_dirs)
 */
void remove_directory(const char *path) {
    DIR *dir = opendir(path);
    struct dirent *entry;
    char fullPath[512];

    // Directory might have been deleted by another thread
    if (dir == NULL) {
        if (errno == ENOENT) {
            return; // Already deleted, not an error
        }
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Create the full path to the file or subdirectory
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        // Check if it's a directory or a file
        struct stat statbuf;
        if (stat(fullPath, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                // If it's a directory, recursively remove its contents
                remove_directory(fullPath);
                if (rmdir(fullPath) != 0 && errno != ENOENT) {
                    perror("Error removing subdirectory");
                }
            } else {
                // If it's a file, remove it
                if (remove(fullPath) != 0 && errno != ENOENT) {
                    perror("Error removing file");
                }
            }
        } else if (errno != ENOENT) {
            // stat failed for reason other than file not existing
            perror("Error stat'ing file");
        }
    }

    closedir(dir);
}


int rm_file_or_folder(socket_md_t* sock) {
    const char* filepath = sock->first_filepath;
    const char* filename = sock->first_filename;
    const char* path = sock->first_dirs;
    
    // LOCK: Protect entire filesystem operation
    pthread_mutex_lock(&utils_mutex);
    
    struct stat statbuf;
    
    // Check if path exists and determine if it's a file or directory
    if (stat(filepath, &statbuf) == 0) {
        if (S_ISREG(statbuf.st_mode)) {
            // It's a regular file
            printf("File '%s' exists\n", filename);
            
            mode_t mode = 0755;
            
            // Change permissions if needed
            if (chmod(filepath, mode) != 0) {
                perror("Error setting file permissions");
                pthread_mutex_unlock(&utils_mutex);
                return -1;
            }
            
            printf("Permissions set to '%o' for '%s'\n", mode, filename);
            
            // Remove the file
            if (remove(filepath) == 0) {
                printf("File '%s' has been deleted successfully\n", filename);
                pthread_mutex_unlock(&utils_mutex);
                return 1;
            } else {
                perror("Error deleting the file");
                pthread_mutex_unlock(&utils_mutex);
                return -1;
            }
        } else if (S_ISDIR(statbuf.st_mode)) {
            // It's a directory
            printf("Directory '%s' exists.\n", path);
            
            mode_t mode = 0755;
            if (chmod(path, mode) != 0) {
                perror("Error changing directory permissions");
                pthread_mutex_unlock(&utils_mutex);
                return -1;
            }
            
            printf("Directory permissions changed to '%o'\n", mode);
            
            // Remove directory and contents
            remove_directory(path);
            
            if (rmdir(path) == 0) {
                printf("Directory '%s' has been removed successfully.\n", path);
                pthread_mutex_unlock(&utils_mutex);
                return 1;
            } else {
                if (errno == ENOENT) {
                    // Already deleted by another thread
                    printf("Directory '%s' was already removed.\n", path);
                    pthread_mutex_unlock(&utils_mutex);
                    return 1;
                }
                perror("Error removing the directory");
                pthread_mutex_unlock(&utils_mutex);
                return -1;
            }
        }
    } else {
        // stat failed
        if (errno == ENOENT) {
            printf("Path '%s' does not exist\n", filepath);
        } else {
            perror("Error accessing path");
        }
        pthread_mutex_unlock(&utils_mutex);
        return -1;
    }
    
    pthread_mutex_unlock(&utils_mutex);
    return 1;
}


int send_msg(int sock_fd, const char* message) {
  // printf("%s\n", server_message); // check
  ssize_t sent_size = send(sock_fd, message, strlen(message), 0);
  if (sent_size < 0) {
    printf("Can't send message\n");
    return -1;
  }
  return sent_size;
}


int send_file(socket_md_t* sock, int sock_fd) {
    if (sock == NULL) {
        fprintf(stderr, "WARNING: file send - socket is NULL\n");
        return -1;
    }
    if (sock->first_filepath == NULL) {
        fprintf(stderr, "WARNING: file send - read filename is NULL\n");
        return -1;
    }
    if (sock_fd < 0) {
        fprintf(stderr, "WARNING: file send - socket file descriptor is invalid\n");
        return -1;
    }

    // Lock the socket mutex to ensure thread-safety when working with the file and socket
    pthread_mutex_lock(&utils_mutex);
    // printf("Local File path: %s\n", file_path);
    FILE *file = fopen(sock->first_filepath, "rb"); // "rb" for read binary
    if (file == NULL) {
        fprintf(stderr, "WARNING: file send - Issue opening read file\n");
        pthread_mutex_unlock(&utils_mutex);
        return -1;
    }

    // Get the size of the file
    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "WARNING: file send - Seeking to end of read file\n");
        pthread_mutex_unlock(&utils_mutex);
        fclose(file);
        return -1;
    }
    long file_size = ftell(file);
    if(file_size < 0) {
        fprintf(stderr, "WARNING: file send - Getting read file size\n");
        pthread_mutex_unlock(&utils_mutex);
        fclose(file);
        return -1;
    }
    if (fseek(file, 0, SEEK_SET) != 0) { // reset the file pointer to the beginning
        fprintf(stderr, "WARNING: file send - Seeking to start of read file\n");
        pthread_mutex_unlock(&utils_mutex);
        fclose(file);
        return -1;
    }
    
    // send the file size
    uint32_t size = htonl(file_size);
    if (send(sock_fd, &size, sizeof(size), 0) < 0) {
        fprintf(stderr, "WARNING: file send - Sending file size\n");
        pthread_mutex_unlock(&utils_mutex);
        fclose(file);
        return -1;
    }

    // send the file data
    char buffer[CHUNK_SIZE]; // buffer to hold file chunks
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) { // reads the given amount of data (CHUNK_SIZE) from the file into the buffer
        size_t total_sent = 0;

        while (total_sent < bytes_read) {
            size_t sent = send(sock_fd, buffer + total_sent,
                                bytes_read - total_sent, 0);

            if (sent < 0) {
                fprintf(stderr, "ERROR: file send - Unable to send message\n");
                pthread_mutex_unlock(&utils_mutex);
                // handle error (disconnect, etc.)
                break;
            }

            printf("Bytes Sent: %lu\n", sent);

            total_sent += sent;
        }
    }

    fclose(file);
    pthread_mutex_unlock(&utils_mutex);
    return 0;
}

int rcv_file(socket_md_t* sock, int sock_fd) {
    if (sock == NULL) {
        fprintf(stderr, "WARNING: file receive - Socket is NULL\n");
        return -1;
    }
    if (sock->sec_filepath == NULL) {
        fprintf(stderr, "WARNING: file receive - Write filename is NULL\n");
        return -1;

    }
    if (sock_fd < 0) {
        fprintf(stderr, "WARNING: file receive - Socket file descriptor is invalid\n");
        return -1;
    }

    // Locking the mutex to protect shared resources like socket descriptor and metadata
    pthread_mutex_lock(&utils_mutex);

    uint32_t size;
    if (recv(sock_fd, &size, sizeof(size), 0) <= 0) {
        perror("ERROR: file receive - Error receiving file size\n");
        pthread_mutex_unlock(&utils_mutex);
        return -1;
    }
    size = ntohl(size);
    printf("File Size: %u\n", size);

    if (size == 0) {
        printf("ERROR: file receive - Received file size is 0\n");
        pthread_mutex_unlock(&utils_mutex);
        return -1;
    }

    FILE *out_file = fopen(sock->sec_filepath, "wb");
    if (out_file == NULL) {
        perror("ERROR: file receive - Error opening write file\n");
        pthread_mutex_unlock(&utils_mutex);
        return -1;
    }

    char buffer[CHUNK_SIZE];
    ssize_t received;
    uint32_t total_received = 0;  // To track total bytes received

    // Loop through and receive the file in chunks
    while (total_received < size) {
        received = recv(sock_fd, buffer, CHUNK_SIZE, 0);
        if (received < 0) {
            perror("ERROR: file receive - Error receiving data\n");
            fclose(out_file);
            pthread_mutex_unlock(&utils_mutex);
            return -1;
        } else if (received == 0) {
            // No more data, but the total received doesn't match the expected size
            fprintf(stderr, "Warning: file receive - Connection closed prematurely\n");
            break;
        }

        // Write the received data to the file
        size_t written = fwrite(buffer, 1, received, out_file);
        if (written != received) {
            perror("ERROR: file receive - Error writing to file\n");
            fclose(out_file);
            pthread_mutex_unlock(&utils_mutex);
            return -1;
        }

        total_received += received;

        // Check if we have received all the expected bytes
        if (total_received > size) {
            fprintf(stderr, "ERROR: file receive - More data received than expected\n");
            fclose(out_file);
            pthread_mutex_unlock(&utils_mutex);
            return -1;
        }

        // Optionally print progress (to debug or monitor)
        printf("Received %u/%u bytes\n", total_received, size);
    }

    fclose(out_file);

    printf("File received successfully!\n");
    pthread_mutex_unlock(&utils_mutex);
    return received;
}

