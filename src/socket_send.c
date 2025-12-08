/**
 * @file socket_send.c / source code for Socket send.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "socket_send.h"

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER; // FIXME: maybe change name


/*
send_msg
*/
ssize_t send_msg(int sock_fd, const char* message) {
  // printf("%s\n", server_message); // check
  ssize_t msg_sent_bytes = send(sock_fd, message, strlen(message), 0);
  if (msg_sent_bytes < 0) {
    printf("Can't send message\n");
    return -1;
  }
  return msg_sent_bytes;
}


/*
send_file_size
*/
ssize_t send_file_size(int sock_fd, long file_size) {
    if (sock_fd < 0) {
        fprintf(stderr,  "WARNING: socket file descriptor is invalid for sending file size\n");
        return -1;
    }
    if (file_size == 0) {
        fprintf(stderr,  "WARNING: file size to send is 0\n");
        return -1;
    }

    // send the file size
    printf("File Size: %ld\n", file_size);
    uint32_t size_in_network_order = htonl(file_size);
    ssize_t bytes_sent = send(sock_fd, &size_in_network_order, sizeof(size_in_network_order), 0);
    if (bytes_sent < 0) {
        printf("Can't send message\n");
        return -1;
    }
    return bytes_sent;
}


/*
send_file
*/
ssize_t send_file(const char* first_filepath, int sock_fd) {
    if (first_filepath == NULL) {
        fprintf(stderr, "WARNING: file send - read filename is NULL\n");
        return -1;
    }
    if (sock_fd < 0) {
        fprintf(stderr, "WARNING: file send - socket file descriptor is invalid\n");
        return -1;
    }

    // Lock the socket mutex to ensure thread-safety when working with the file and socket
    pthread_mutex_lock(&send_mutex);
    // printf("Local File path: %s\n", file_path);
    FILE *file = fopen(first_filepath, "rb"); // "rb" for read binary
    if (file == NULL) {
        fprintf(stderr, "WARNING: file send - Issue opening read file\n");
        pthread_mutex_unlock(&send_mutex);
        return -1;
    }

    // send the file data
    char buffer[CHUNK_SIZE]; // buffer to hold file chunks
    ssize_t bytes_read;
    ssize_t final_bytes_sent = 0;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) { // reads the given amount of data (CHUNK_SIZE) from the file into the buffer
        ssize_t total_bytes_sent = 0;

        while (total_bytes_sent < bytes_read) {
            ssize_t bytes_sent = send(sock_fd, buffer + total_bytes_sent,
                                bytes_read - total_bytes_sent, 0);

            if (bytes_sent < 0) {
                fprintf(stderr, "ERROR: file send - Unable to send message\n");
                pthread_mutex_unlock(&send_mutex);
                // handle error (disconnect, etc.)
                break;
            }

            printf("Bytes Sent: %lu\n", bytes_sent);

            total_bytes_sent += bytes_sent;
        }
        final_bytes_sent = total_bytes_sent;
    }

    fclose(file);
    pthread_mutex_unlock(&send_mutex);
    return final_bytes_sent;
}
