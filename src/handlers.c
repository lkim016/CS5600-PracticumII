/**
 * @file handlers.c / source code for request handlers.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "handlers.h"

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
push
NOTE: need to free() result
*/
char* deliver(unsigned long t_id, int sock_fd, char* filepath1, uint32_t file_size) {
    // check if file_exits - if yes then send and receive
    char* msg = NULL;
    printf("First file exists with size of %u.\n", file_size);
    pthread_mutex_lock(&file_mutex);  // Lock filesystem
    if (file_exists(filepath1) == 1 && file_size > 0) {
        int size_sent_status = send_size(sock_fd, file_size);
        pthread_mutex_unlock(&file_mutex);  // Lock filesystem
        if (size_sent_status == 0) {
            printf("Sending file data (%u bytes)...\n", file_size);
            if (send_file(filepath1, sock_fd) < 0) {
                msg = build_send_msg(t_id, "", "File send failed");
                return msg;
            } else {
                msg = build_send_msg(t_id, "", "File was sent!");
                return msg;
            }
        } else {
            msg = build_send_msg(t_id, "", "File did sent...");
            return msg;
        }
    } else {
        msg = build_send_msg(t_id, "", "File does not exist.");
    }

    msg = build_send_msg(t_id, "", "Message is empty");
    return msg;
}

/*
pull
NOTE: need to free() result
*/
char* receive(unsigned long t_id, int sock_fd, char* filepath2, uint32_t file_size) {
    char* msg = NULL;
    int folder_exists = folder_not_exists_make(filepath2);
    if (folder_exists == 0 && file_size > 0) {
        printf("Path existed or was newly created\n");

        printf("Receiving file (%u bytes) to: %s\n", file_size, filepath2);
        ssize_t file_rcvd_bytes = rcv_file(sock_fd, filepath2, file_size);
        if (file_rcvd_bytes < 0 ) {
            msg = build_send_msg(t_id, "", "Error receiving file");
        } else {
            msg = build_send_msg(t_id, "", "File was successfully received!");
        }
    } else {
        if(file_size == 0) {
            msg = build_send_msg(t_id, "", "Error size of file being sent is 0");
        } else {
            msg = build_send_msg(t_id, "", "Error file size is 0 or folder was not able to be made");
        }
    }
    
    msg = build_send_msg(t_id, "", "Message is empty");
    return msg;
}

/*
build_send_msg
NOTE: need to free() result
*/
char* build_send_msg(unsigned long id, const char* part1, const char* part2) { // FIXME: change to build_send_msg
    if (id < 0) {
        return NULL;
    }
    if (part1 == NULL) {
        return NULL;
    }
    if (part2 == NULL) {
        return NULL;
    }

    size_t len = strlen(part1) + strlen(part2) + MSG_SIZE; // 1 for space, 1 for newline, 1 for null terminator
    char* msg_ptr = calloc(len, sizeof(char));
    if (msg_ptr == NULL) {
        perror("Memory allocation failed\n");
        return NULL;
    }
    sprintf(msg_ptr, "Thread ID# %lu:\n  %s %s\n", id, part1, part2);
    return msg_ptr;
}
