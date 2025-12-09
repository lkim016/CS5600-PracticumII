/**
 * @file handlers.c / source code for request handlers.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "handlers.h"

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

void push(int sock_fd, char* filepath1, uint32_t file_size) {
    // check if file_exits - if yes then send and receive
    
    printf("First file exists with size of %u.\n", file_size);
    if (file_exists(filepath1) == 1 && file_size > 0) {
        int size_sent_status = send_size(sock_fd, file_size);
        if (size_sent_status == 0) {
            printf("Sending file data (%u bytes)...\n", file_size);
            if (send_file(filepath1, sock_fd) < 0) {
                fprintf(stderr, "File send failed\n");
            } else {
                printf("File sent..\n");
            }
        }
    } else {
        fprintf(stderr, "File does not exist.\n");
    }
}

void pull(int sock_fd, char* filepath2, uint32_t file_size) {
    pthread_mutex_lock(&file_mutex);  // Lock filesystem
    int folder_exists = folder_not_exists_make(filepath2);
    if (folder_exists == 0 && file_size > 0) {
        printf("Path existed or was newly created\n");

        printf("Receiving file (%u bytes) to: %s\n", file_size, filepath2);
        ssize_t file_rcvd_bytes = rcv_file(sock_fd, filepath2, file_size);
        pthread_mutex_unlock(&file_mutex);  // Lock filesystem
        if (file_rcvd_bytes < 0 ) {
            printf("%s\n", "Error receiving file");
        }

        printf("%s\n", "File sent successfully!");
    } else {
        if(file_size == 0) {
            printf("%s\n", "Error size of file being sent is 0");
        } else {
            printf("%s\n", "Error file size is 0 or folder was not able to be made");
        }
    }

}
