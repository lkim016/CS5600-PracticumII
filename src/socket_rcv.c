/**
 * @file socket_rcv.c / source code for Socket receive.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "socket_rcv.h"

pthread_mutex_t rcv_mutex = PTHREAD_MUTEX_INITIALIZER; // FIXME: maybe change name


/*
rcv_file_size
*/
uint32_t rcv_file_size(int sock_fd) {
    if (sock_fd < 0) {
        fprintf(stderr, "WARNING: socket file descriptor is invalid for receiving file size\n");
        return 0;
    }
    uint32_t size;
    if (recv(sock_fd, &size, sizeof(size), 0) <= 0) {
        perror("ERROR: Error receiving file size\n");
        return 0;
    }
    size = ntohl(size);
    return size;
}

/*
rcv_file
*/
ssize_t rcv_file(const char* sec_filepath, int sock_fd, uint32_t size) {

    if (sec_filepath == NULL) {
        fprintf(stderr, "WARNING: file receive - Write filename is NULL\n");
        return -1;

    }
    if (sock_fd < 0) {
        fprintf(stderr, "WARNING: file receive - Socket file descriptor is invalid\n");
        return -1;
    }
    if (size == 0) {
        return -1;
    }

    // Locking the mutex to protect shared resources like socket descriptor and metadata
    pthread_mutex_lock(&rcv_mutex);
    FILE *out_file = fopen(sec_filepath, "wb");
    if (out_file == NULL) {
        perror("ERROR: file receive - Error opening write file\n");
        pthread_mutex_unlock(&rcv_mutex);
        return -1;
    }

    char buffer[CHUNK_SIZE];
    ssize_t rcvd_bytes;
    uint32_t total_received = 0;  // To track total bytes received

    // Loop through and receive the file in chunks
    while (total_received < size) {
        rcvd_bytes = recv(sock_fd, buffer, CHUNK_SIZE, 0);
        if (rcvd_bytes < 0) {
            perror("ERROR: file receive - Error receiving data\n");
            fclose(out_file);
            pthread_mutex_unlock(&rcv_mutex);
            return -1;
        } else if (rcvd_bytes == 0) {
            // No more data, but the total received doesn't match the expected size
            fprintf(stderr, "Warning: file receive - Connection closed prematurely\n");
            break;
        }

        // Write the received data to the file
        size_t written = fwrite(buffer, 1, rcvd_bytes, out_file);
        if (written != rcvd_bytes) {
            perror("ERROR: file receive - Error writing to file\n");
            fclose(out_file);
            pthread_mutex_unlock(&rcv_mutex);
            return -1;
        }

        total_received += rcvd_bytes;

        // Check if we have received all the expected bytes
        if (total_received > size) {
            fprintf(stderr, "ERROR: file receive - More data received than expected\n");
            fclose(out_file);
            pthread_mutex_unlock(&rcv_mutex);
            return -1;
        }

        // Optionally print progress (to debug or monitor)
        printf("Received %u/%u bytes\n", total_received, size);
    }

    fclose(out_file);

    printf("File received successfully!\n");
    pthread_mutex_unlock(&rcv_mutex);
    return total_received;
}