/**
 * @file utils.c / source code for program utilities.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "utils.h"


int folder_not_exists_make(const char* file_path) {
    // Find the last occurrence of the directory separator - Copy the directory part
    const char *last_slash = strrchr(file_path, SINGLE_PATH_DELIMITER);
    size_t dir_len = last_slash - file_path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
    char* dirs = (char*)calloc(dir_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
    if (dirs == NULL) {
        // Handle memory allocation failure if needed
        perror("calloc failed for first_dirs\n");
        return -1;
    }
    strncpy(dirs, file_path, dir_len);  // Copy the directory part into first_dirs
    
    if (file_path == NULL) {
        printf("Invalid path!\n");
        return -1; // Invalid path
    }
    // loop through folder_path with strtok
    char* path = (char*)calloc(1, sizeof(char));
    if (path == NULL) {
        printf("Memory allocation failed!\n");
        return -1;
    }
    path[0] = '\0';
    
    char* token = strtok(strdup(dirs), "/");
    while (token != NULL) {
        // Concatenate the token to the current path
        size_t new_size = strlen(path) + strlen(token) + 2; // +2 for '/' and '\0'
        char* temp = realloc(path, new_size);
        if (temp == NULL) {
            printf("Memory reallocation failed!\n");
            free(path);
            return -1;
        }
        path = temp;

        // Add the token to the path
        strcat(path, token);
        printf("Checking path: %s\n", path);

        // Try to open the directory
        DIR *dir = opendir(path);
        // If opendir returns NULL, the directory doesn't exist
        if (dir) {
            // Directory exists, so close the directory and return true
            closedir(dir);
        } else {
            // Directory doesn't exist, create it
            if (mkdir(path, 0755) != 0) {
                printf("Failed to create directory: %s\n", path);
                free(path);
                return -1;
            }
        }
        
        token = strtok(NULL, "/");
    }

    free(dirs);
    free(path);
    return 1;
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

    if (dir == NULL) {
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
                rmdir(fullPath);  // Remove the empty subdirectory after deleting its contents
            } else {
                // If it's a file, remove it
                remove(fullPath);
            }
        }
    }

    closedir(dir);
}


int rm_file_or_folder(socket_t* sock) {
    const char* filepath = sock->first_filepath;
    const char* filename = sock->first_filename;
    const char* path = sock->first_dirs;
    FILE *file = fopen(filepath, "r");  // Try to open the file in read mode

    if (file) {
        printf("File '%s' exists\n", filename);
        fclose(file);  // Close the file after checking

        mode_t mode = 0755; // Full permissions for the owner, read and execute for others.
        // Set the directory permissions
        if (chmod(path, mode) == 0) {
            printf("Directory permissions changed to '%o'\n", mode);
        } else {
            perror("Error changing directory permissions");
            return -1;
        }

        if(chmod(filepath, mode) == 0) {
            printf("Permissions set to '%o' for '%s'\n", mode, filename);
            // Remove the file

            if (remove(filepath) == 0) {
                printf("File '%s' has been deleted successfully\n", filename);
                return 1;
            } else {
                perror("Error deleting the file\n");
                return -1;
            }
        } else {
            perror("Error setting file permissions\n");
            return -1;
        }
    } else {
        printf("File '%s' does not exist\n", filename);
        // Try to open the directory
        DIR *dir = opendir(path);
        if (dir) {
            printf("Directory '%s' exists.\n", path);
            closedir(dir);  // Close the directory when done

            remove_directory(path);

            if (rmdir(path) == 0) {  // Finally, remove the empty directory
                printf("Directory '%s' has been removed successfully.\n", path);
            } else {
                perror("Error removing the directory\n");
            }
            return 1;
        } else {
            printf("Directory '%s' does not exist or cannot be opened\n", path);
            return -1;
        }
    }

    return 0;
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


void send_file(socket_t* sock, int sock_fd) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        free_socket(sock);
        exit(1);
    }
    if (sock->first_filepath == NULL) {
        fprintf(stderr, "ERROR: read filename is NULL\n");
        free_socket(sock);
        exit(1);

    }
    if (sock_fd < 0) {
        fprintf(stderr, "ERROR: socket file descriptor is invalid\n");
        free_socket(sock);
        exit(1);
    }

    // printf("Local File path: %s\n", file_path);
    FILE *file = fopen(sock->first_filepath, "rb"); // "rb" for read binary
    if (file == NULL) {
        perror("Error opening read file\n");
        free_socket(sock);
        exit(1);
    }

    // Get the size of the file
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error seeking to end of read file\n");
        free_socket(sock);
        fclose(file);
        exit(1);
    }
    long file_size = ftell(file);
    if(file_size < 0) {
        perror("Error getting read file size\n");
        free_socket(sock);
        fclose(file);
        exit(1);

    }
    if (fseek(file, 0, SEEK_SET) != 0) { // reset the file pointer to the beginning
        perror("Error seeking to start of read file\n");
        free_socket(sock);
        fclose(file);
        exit(1);
    }
    
    // send the file size
    uint32_t size = htonl(file_size);
    if (send(sock_fd, &size, sizeof(size), 0) < 0) {
        perror("Error sending file size\n");
        fclose(file);
        free_socket(sock);
        exit(1);
    }

    // send the file data
    char buffer[CHUNK_SIZE]; // buffer to hold file chunks
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) { // reads the given amount of data (CHUNK_SIZE) from the file into the buffer
    size_t total_sent = 0;

    while (total_sent < bytes_read) {
        ssize_t sent = send(sock_fd, buffer + total_sent,
                            bytes_read - total_sent, 0);

        if (sent < 0) {
            perror("Unable to send message\n");
            // handle error (disconnect, etc.)
            fclose(file);
            free_socket(sock);
            exit(1);
        }

        printf("Bytes Sent: %lu\n", sent);

        total_sent += sent;
        }
    }

    fclose(file);
    
}


int rcv_file(socket_t* sock, int sock_fd) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL\n");
        return -1;
    }
    if (sock->sec_filepath == NULL) {
        fprintf(stderr, "ERROR: write filename is NULL\n");
        return -1;

    }
    if (sock_fd < 0) {
        fprintf(stderr, "ERROR: socket file descriptor is invalid\n");
        return -1;
    }

    uint32_t size;
    if (recv(sock_fd, &size, sizeof(size), 0) <= 0) {
        perror("Error receiving file size\n");
        return -1;
    }
    size = ntohl(size);
    printf("File Size: %u\n", size);

    if (size == 0) {
        printf("Received file size is 0. No file to receive.\n");
        return -1;
    }

    FILE *out_file = fopen(sock->sec_filepath, "wb");
    if (out_file == NULL) {
        perror("Error opening write file\n");
        return -1;
    }

    char buffer[CHUNK_SIZE];
    ssize_t received;
    uint32_t total_received = 0;  // To track total bytes received

    // Loop through and receive the file in chunks
    while (total_received < size) {
        received = recv(sock_fd, buffer, CHUNK_SIZE, 0);
        if (received < 0) {
            perror("Error receiving data\n");
            fclose(out_file);
            return -1;
        } else if (received == 0) {
            // No more data, but the total received doesn't match the expected size
            fprintf(stderr, "Warning: Connection closed prematurely\n");
            break;
        }

        // Write the received data to the file
        size_t written = fwrite(buffer, 1, received, out_file);
        if (written != received) {
            perror("Error writing to file\n");
            fclose(out_file);
            return -1;
        }

        total_received += received;

        // Check if we have received all the expected bytes
        if (total_received > size) {
            fprintf(stderr, "Error: More data received than expected\n");
            fclose(out_file);
            return -1;
        }

        // Optionally print progress (to debug or monitor)
        printf("Received %u/%u bytes\n", total_received, size);
    }

    fclose(out_file);

    printf("File received successfully!\n");
    return received;
}

