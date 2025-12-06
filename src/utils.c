/**
 * @file utils.c / source code for program utilities.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Nov 11, 2025 / Fall 2025
 *
*/

#include "utils.h"

void split_read_path(const char *path, socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }
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


void split_write_path(const char *path, socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }
    // Find the last occurrence of the directory separator
    const char *last_sep = strrchr(path, PATH_DELIMITER);
    size_t dir_len = 0;
    if (last_sep != NULL) {
        // Copy the directory part
        dir_len = last_sep - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
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
        dir_len = strlen(path);  // Directory length will be the full length of the path
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