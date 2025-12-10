/**
 * @file utils.c / source code for file/folder utilities.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "utils.h"

pthread_mutex_t utils_mutex = PTHREAD_MUTEX_INITIALIZER;  // Filesystem operations mutex - maybe change name to files_mutex


/*
file_exists
*/
bool file_exists(const char* file) {
    return access(file, F_OK) == 0;
}


/*
get_file_size
*/
uint32_t get_file_size(const char* filepath) {
    if(filepath == NULL) {
        return -1;
    }

    uint32_t filesize32 = 0;
    struct stat st;
    if (stat(filepath, &st) == 0) {
        if (st.st_size < 0 || st.st_size > UINT32_MAX) {
            fprintf(stderr, "File too large for 32-bit protocol\n");
            return -1;
        }

        filesize32 = (uint32_t) st.st_size;
    }
    return filesize32;
}

/*
folder_not_exists_make
*/
int folder_not_exists_make(const char* file_path) {
    if (file_path == NULL) {
        fprintf(stderr, "Invalid path!\n");
        return -1;
    }

    char *path_copy = strdup(file_path);
    if (!path_copy) {
        fprintf(stderr, "strdup failed\n");
        return -1;
    }
    const char *last_slash = strrchr(path_copy, LITERAL_PATH_DELIMITER);
    if (last_slash == NULL) {
        free(path_copy);
        return 0; // No directory component
    }
    // Copy the directory part
    size_t dir_len = last_slash - path_copy + 1;  // pointer arithmetic calculating the length of the directory part of the path string
    char* dirs = (char*)calloc(dir_len + 1, sizeof(char)); // Allocate memory for the directory part, including the null terminator
    if (dirs == NULL) {
        // Handle memory allocation failure if needed
        fprintf(stderr, "calloc failed for setting dirs\n");
        free(path_copy);
        return -1;
    }
    strncpy(dirs, path_copy, dir_len);  // Copy the directory part into sec_dirs
    dirs[dir_len] = '\0';  // Manually null-terminate the string

    size_t path_buf_size = dir_len + 256; // extra space for concatenation
    char *path = (char*)calloc(path_buf_size, sizeof(char));
    if (path == NULL) {
        fprintf(stderr, "calloc failed for path\n");
        free(path_copy);
        free(dirs);
        return -1;
    }
    path[0] = '\0'; // start empty
    char *token = strtok(dirs, "/");
    while (token != NULL) {
        // Append the token + delimiter safely using snprintf
        int n = snprintf(path + strlen(path), path_buf_size - strlen(path), "%s/", token);
        if (n < 0 || (size_t)n >= path_buf_size - strlen(path)) {
            fprintf(stderr, "Path buffer overflow\n");
            free(path_copy);
            free(dirs);
            free(path);
            return -1;
        }
        // Create directory if it doesn't exist
        struct stat st;
        if (stat(path, &st) != 0) {
            // Directory doesn't exist, create it
            if (mkdir(path, 0755) != 0) {
                if (errno != EEXIST) {
                    fprintf(stderr, "Failed to create directory: %s\n", path);
                    free(path_copy);
                    free(dirs);
                    free(path);
                    return -1;
                }
            }
        } else if (!S_ISDIR(st.st_mode)) {
            // Path exists but is not a directory
            fprintf(stderr, "Path exists but is not a directory: %s\n", path);
            free(path_copy);
            free(dirs);
            free(path);
            return -1;
        }

        token = strtok(NULL, PATH_DELIMITER);
    }
    free(path_copy);
    free(dirs);
    free(path);
    
    printf("Directory check complete\n");
    
    return 0;
}

/**
 * @brief Function to remove a directory and all of its contents for command RM
 *
 * @param path const char* - the socket metadata object's folder path (write_dirs)
 */
void remove_directory(const char *path) {
    if (path == NULL) {
        fprintf(stderr, "Path is NULL\n");
        return;
    }
    DIR *dir = opendir(path);
    struct dirent *entry;
    char fullPath[512];

    // Directory might have been deleted by another thread
    if (dir == NULL) {
        if (errno == ENOENT) {
            return; // Already deleted, not an error
        }
        perror("Error opening directory\n");
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

/*
rm_file_or_folder
*/
int rm_file_or_folder(socket_md_t* sock) {
    if (sock == NULL) {
        return -1;
    }
    char* fp1 = sock->first_filepath;
    char* fn1 = sock->first_filename;
    char* dirs1 = sock->first_dirs;
    
    char* filepath = NULL;
    char* filename = NULL;
    char* path = NULL;
    if (fp1 != NULL) {
        filepath = strdup(fp1);
    }
    if (fn1 != NULL) {
        filename = strdup(fn1);
    }
    if (dirs1 != NULL) {
        path = strdup(dirs1);
    }
    
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
                free(filepath);
                free(filename);
                free(path);
                pthread_mutex_unlock(&utils_mutex);
                return -1;
            }
            
            printf("Permissions set to '%o' for '%s'\n", mode, filename);
            
            // Remove the file
            if (remove(filepath) == 0) {
                printf("File '%s' has been deleted successfully\n", filename);
                free(filepath);
                free(filename);
                free(path);
                pthread_mutex_unlock(&utils_mutex);
                return 1;
            } else {
                perror("Error deleting the file");
                free(filepath);
                free(filename);
                free(path);
                pthread_mutex_unlock(&utils_mutex);
                return -1;
            }
        } else if (S_ISDIR(statbuf.st_mode)) {
            // It's a directory
            printf("Directory '%s' exists.\n", path);
            
            mode_t mode = 0755;
            if (chmod(path, mode) != 0) {
                perror("Error changing directory permissions");
                free(filepath);
                free(filename);
                free(path);
                pthread_mutex_unlock(&utils_mutex);
                return -1;
            }
            
            printf("Directory permissions changed to '%o'\n", mode);
            
            // Remove directory and contents
            remove_directory(path);
            
            if (rmdir(path) == 0) {
                printf("Directory '%s' has been removed successfully.\n", path);
                free(filepath);
                free(filename);
                free(path);
                pthread_mutex_unlock(&utils_mutex);
                return 1;
            } else {
                if (errno == ENOENT) {
                    // Already deleted by another thread
                    printf("Directory '%s' was already removed.\n", path);
                    free(filepath);
                    free(filename);
                    free(path);
                    pthread_mutex_unlock(&utils_mutex);
                    return 1;
                }
                perror("Error removing the directory");
                free(filepath);
                free(filename);
                free(path);
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
        free(filepath);
        free(filename);
        free(path);
        pthread_mutex_unlock(&utils_mutex);
        return -1;
    }
    
    free(filepath);
    free(filename);
    free(path);
    pthread_mutex_unlock(&utils_mutex);
    return 1;
}



