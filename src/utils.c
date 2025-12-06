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
    const char *last_slash = strrchr(path, PATH_DELIMITER);
    size_t dir_len = 0;
    size_t ext_len = 0;
    const char *last_dot = NULL;
    if (last_slash != NULL) {
        // Copy the directory part
        dir_len = last_slash - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
        sock->read_pdir = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->read_pdir == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for read_pdir");
            exit(1);
        }
        strncpy(sock->read_pdir, path, dir_len);  // Copy the directory part into read_pdir
        // sock->read_pdir[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Copy the filename part (after the last separator)
        sock->read_filename = (char*)calloc(strlen(last_slash + 1) + 1, sizeof(char));  // Allocate memory for the filename part
        if (sock->read_filename == NULL) {
            perror("calloc failed for read_filename");
            exit(1);
        }
        strcpy(sock->read_filename, last_slash + 1);  // Copy the filename part into read_filename

        // Separate out the extension from the filename
        last_dot = strrchr(sock->read_filename, '.');  // Find the last period in the filename
        if (last_dot != NULL) {
            ext_len = last_dot - sock->read_filename + 1;  // pointer arithmetic calculating the length of the file ext part of the filename
            sock->read_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
            if (sock->read_file_ext == NULL) {
                perror("calloc failed for read_file_ext");
                exit(1);
            }
            // If a period is found, extract the extension
            sock->read_file_ext = strdup(last_dot + 1);  // Copy the extension into read_file_ext (without the period)
        }

    } else {
        // If no directory separator is found, the whole path is the filename
        dir_len = strlen(path);  // Directory length will be the full length of the path
        sock->read_filename = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the full filename
        if (sock->read_filename == NULL) {
            perror("calloc failed for read_filename (no separator)");
            exit(1);
        }
        strcpy(sock->read_filename, path);  // Copy the whole path into read_filename

        // Separate out the extension from the filename
        last_dot = strrchr(sock->read_filename, '.');  // Find the last period in the filename
        if (last_dot != NULL) {
            ext_len = last_dot - sock->read_filename + 1;  // pointer arithmetic calculating the length of the file ext part of the filename
            sock->read_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
            if (sock->read_file_ext == NULL) {
                perror("calloc failed for read_file_ext");
                exit(1);
            }
            // If a period is found, extract the extension
            sock->read_file_ext = strdup(last_dot + 1);  // Copy the extension into read_file_ext (without the period)
        } else {
            ext_len = strlen(DEFAULT_FILE_EXT);
            sock->read_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
            sock->read_file_ext = strdup(DEFAULT_FILE_EXT); // Copy the default extension into read_file_ext
        }
    }

}


void split_write_path(const char *path, socket_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "ERROR: socket is NULL");
        exit(1);
    }
    // Find the last occurrence of the directory separator
    const char *last_slash = strrchr(path, PATH_DELIMITER);
    size_t dir_len = 0;
    size_t ext_len = 0;
    const char *last_dot = NULL;
    if (last_slash != NULL) {
        // Copy the directory part
        dir_len = last_slash - path + 1;  // pointer arithmetic calculating the length of the directory part of the path string
        sock->write_pdir = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
        if (sock->write_pdir == NULL) {
            // Handle memory allocation failure if needed
            perror("calloc failed for write_pdir");
            exit(1);
        }
        strncpy(sock->write_pdir, path, dir_len);  // Copy the directory part into write_pdir
        // sock->write_pdir[dir_len] = '\0';  // Null-terminate (not strictly necessary as calloc initializes memory to zero)

        // Copy the filename part (after the last separator)
        sock->write_filename = (char*)calloc(strlen(last_slash + 1) + 1, sizeof(char));  // Allocate memory for the filename part
        if (sock->write_filename == NULL) {
            perror("calloc failed for write_filename");
            exit(1);
        }
        strcpy(sock->write_filename, last_slash + 1);  // Copy the filename part into write_filename

        // Separate out the extension from the filename
        last_dot = strrchr(sock->write_filename, '.');  // Find the last period in the filename
        if (last_dot != NULL) {
            ext_len = last_dot - sock->write_filename + 1;  // pointer arithmetic calculating the length of the file ext part of the filename
            sock->write_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
            if (sock->write_file_ext == NULL) {
                perror("calloc failed for write_file_ext");
                exit(1);
            }
            // If a period is found, extract the extension
            sock->write_file_ext = strdup(last_dot + 1);  // Copy the extension into write_file_ext (without the period)
        }

    } else {
        // If no directory separator is found, the whole path is the filename
        dir_len = strlen(path);  // Directory length will be the full length of the path
        sock->write_filename = (char*)calloc(dir_len + 1, sizeof(char));  // Allocate memory for the full filename
        if (sock->write_filename == NULL) {
            perror("calloc failed for write_filename (no separator)");
            exit(1);
        }
        strcpy(sock->write_filename, path);  // Copy the whole path into write_filename

        // Separate out the extension from the filename
        last_dot = strrchr(sock->write_filename, '.');  // Find the last period in the filename
        if (last_dot != NULL) {
            ext_len = last_dot - sock->write_filename + 1;  // pointer arithmetic calculating the length of the file ext part of the filename
            sock->write_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
            if (sock->write_file_ext == NULL) {
                perror("calloc failed for write_file_ext");
                exit(1);
            }
            // If a period is found, extract the extension
            sock->write_file_ext = strdup(last_dot + 1);  // Copy the extension into write_file_ext (without the period)
        } else {
            ext_len = strlen(DEFAULT_FILE_EXT);
            sock->write_file_ext = (char*)calloc(ext_len + 1, sizeof(char));  // Allocate memory for the directory part, including the null terminator
            sock->write_file_ext = strdup(DEFAULT_FILE_EXT); // Copy the default extension into write_file_ext
        }
    }
}