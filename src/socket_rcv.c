/**
 * @file socket_rcv.c / source code for Socket receive.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "protocol.h"
#include "socket_rcv.h"

pthread_mutex_t rcv_mutex = PTHREAD_MUTEX_INITIALIZER; // FIXME: maybe change name
// #define MAX_FILE_SIZE (1024UL * 1024 * 1024)  // 1 GB sanity limit


ssize_t recv_all(int sock_fd, void *buffer, size_t length) {
    if (sock_fd < 0) {
        return 0;
    }
    if (length <= 0) {
        return 0;
    }
    size_t total = 0;
    char *buf = buffer;

    while (total < length) {
        ssize_t n = recv(sock_fd, buf + total, length - total, 0);
        if (n <= 0) return n; // error or disconnect
        total += n;
    }
    return total;
}


/*
rcv_file_size
*/
void rcv_request(socket_md_t* sock) {
    if (sock == NULL) {
        fprintf(stderr, "Socket metadata is NULL for receiving request\n");
        return;
    }

    if (sock->command == STOP) {
        return;
    }

    int sock_fd = sock->client_sock_fd;
    struct header h;

    if (recv_all(sock_fd, &h, sizeof(h)) <= 0) {
        printf("Client disconnected\n");
        return;
    }

    uint32_t temp = 0;
    memcpy(&temp, &h.command, sizeof(uint32_t));
    sock->command = ntohl(temp);
    memcpy(&temp, &h.fpath1_len, sizeof(uint32_t));
    uint32_t fpath1_len = ntohl(temp);
    memcpy(&temp, &h.fpath2_len, sizeof(uint32_t));
    uint32_t fpath2_len = ntohl(temp);
    memcpy(&temp, &h.file_size, sizeof(uint32_t));
    sock->file_size = ntohl(temp);


    /* Read filenames */
    char* fpath1 = malloc(fpath1_len + 1);
    char* fpath2 = malloc(fpath2_len + 1);

    recv_all(sock_fd, fpath1, fpath1_len);
    fpath1[fpath1_len] = '\0';

    recv_all(sock_fd, fpath2, fpath2_len);
    fpath2[fpath2_len] = '\0';


    if(sock->command != STOP) {
        set_first_fileInfo(fpath1, sock);
        set_first_filepath(sock); 
    }
    
    if (fpath2 != NULL) {
        set_sec_fileInfo(fpath2, sock);
    } else if (sock->command != RM || sock->command != STOP) {
        set_sec_fileInfo(fpath1, sock);
    }
    
    if (sock->command != RM || sock->command != STOP) {
        set_sec_filepath(sock);
    }


    printf("Command: %u\n", sock->command);
    printf("Filename1: %s\n", sock->first_filepath);
    printf("Filename2: %s\n", sock->sec_filepath);

    if (fpath1 != NULL) {
        free(fpath1);
    }
    if (fpath2 != NULL) {
        free(fpath2);
    }
    
    return;
}

/*
rcv_file
*/
ssize_t rcv_file(int sock_fd, const char* sec_filepath, uint32_t size) {

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
    ssize_t total_received = 0;  // To track total bytes received

    // Loop through and receive the file in chunks
    while (total_received < size) {
        size_t bytes_to_recv = CHUNK_SIZE;
        if (size - total_received < CHUNK_SIZE)
            bytes_to_recv = size - total_received;

        ssize_t rcvd_bytes = recv(sock_fd, buffer, bytes_to_recv, 0);
        if (rcvd_bytes < 0) {
            perror("ERROR: file receive - Error receiving data\n");
            fclose(out_file);
            pthread_mutex_unlock(&rcv_mutex);
            return -1;
        }

        // Write the received data to the file
        size_t written = fwrite(buffer, 1, rcvd_bytes, out_file);
        if (written != (size_t)rcvd_bytes) {
            perror("ERROR: file receive - Error writing to file\n");
            fclose(out_file);
            pthread_mutex_unlock(&rcv_mutex);
            return -1;
        }

        total_received += rcvd_bytes;

        // Optional: print progress
        printf("Received %lu/%u bytes\r", total_received, size);
        fflush(stdout);
    }

    printf("\nFile receive complete: %lu/%u bytes\n", total_received, size);
    fclose(out_file);
    pthread_mutex_unlock(&rcv_mutex);
    return total_received;
}