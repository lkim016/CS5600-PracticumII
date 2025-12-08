/**
 * @file socket_send.c / source code for Socket send.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Dec 5, 2025 / Fall 2025
 *
*/

#include "protocol.h"
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
int send_request(socket_md_t* sock) {
    if (sock == NULL) {
        fprintf(stderr,  "WARNING: socket protocal is NULL for sending file size\n");
        return -1;
    }
    
    int sock_fd = sock->client_sock_fd;
    int command = sock->command;
    char* first_filepath = sock->first_filepath;
    char* sec_filepath = NULL;
    if (sock->sec_filepath != NULL) {
        sec_filepath = sock->sec_filepath;
    } else {
        sec_filepath[0] = '\0';
    }

    uint32_t fpath1_len = strlen(first_filepath);
    uint32_t fpath2_len = strlen(sec_filepath);
    

    struct header h;
    h.command = htonl(command);
    h.fpath1_len = htonl(fpath1_len);
    h.fpath2_len = htonl(fpath2_len);
    h.file_size = sock->file_size;

    /* Send header */
    if (send(sock_fd, &h, sizeof(h), 0) != sizeof(h)) {
        perror("send header");
        return -1;
    }

    /* Send filenames */
    send(sock_fd, first_filepath, fpath1_len, 0);
    send(sock_fd, sec_filepath, fpath2_len, 0);

    return 0;
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
    ssize_t total = 0;
    while (!feof(file)) {
        ssize_t bytes_read = fread(buffer, 1, CHUNK_SIZE, file);
        if (bytes_read > 0) {
            ssize_t bytes_sent = send(sock_fd, buffer,
                                bytes_read, 0);

            if (bytes_sent <= 0) {
                fprintf(stderr, "ERROR: file send - Unable to send file\n");
                pthread_mutex_unlock(&send_mutex);
                // handle error (disconnect, etc.)
                break;
            }
            total += bytes_sent;
        }

    }
    printf("Bytes Sent: %lu\n", total);

    fclose(file);
    pthread_mutex_unlock(&send_mutex);
    return total;
}
