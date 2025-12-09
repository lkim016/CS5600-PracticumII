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
dyn_msg
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

    size_t len = strlen(part1) + strlen(part2) + 3; // 1 for space, 1 for newline, 1 for null terminator
    char* msg_ptr = calloc(len, sizeof(char));
    if (msg_ptr == NULL) {
        perror("Memory allocation failed\n");
        return NULL;
    }
    sprintf(msg_ptr, "Thread ID#%lu:\n  %s %s\n", id, part1, part2);
    return msg_ptr;
}


/*
send_msg
*/
ssize_t send_msg(int sock_fd, const char* message) {
    if (sock_fd < 0) {
        return -1;
    }
    if (message == NULL) {
        return -1;
    }
    // printf("%s\n", server_message); // check
    ssize_t msg_sent_bytes = send(sock_fd, message, strlen(message), 0);
    if (msg_sent_bytes < 0) {
        printf("Can't send message\n");
        return -1;
    }
    return msg_sent_bytes;
}

/*
send_msg
*/
ssize_t send_size(int sock_fd, uint32_t size) {
    if (sock_fd < 0) {
        fprintf(stderr,  "ERROR: socket fd is invalid, could not send\n");
        return -1;
    }
    if (size == 0) {
        fprintf(stderr,  "ERROR: sending file size is 0, could not send\n");
        return -1;
    }
    
    // set all values in protocol except file size to empty values
    // int sock_fd = sock->client_sock_fd;
    int command = NULL_VAL;
    char* first_filepath = calloc(1, 1); // allocate 1 byte for null terminator
    if (!first_filepath) {
        perror("WARNING: calloc failed for first_filepath, could not send");
        return -1;
    }
    first_filepath = '\0';

    char* sec_filepath = calloc(1, 1); // allocate 1 byte for null terminator
    if (!sec_filepath) {
        perror("WARNING: failed for sec_filepath, could not send");
        return -1;
    }
    sec_filepath = '\0';

    // serialize
    uint32_t fpath1_len = strlen(first_filepath);
    uint32_t fpath2_len = strlen(sec_filepath);
    

    struct header h;
    h.command = htonl(command);
    h.fpath1_len = htonl(fpath1_len);
    h.fpath2_len = htonl(fpath2_len);
    h.file_size = htonl(size); // stored as unit32_t, still need htonl since sending over network

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
send_file_size
*/
int send_request(socket_md_t* sock) {
    if (sock == NULL) {
        fprintf(stderr,  "ERROR: socket protocal is NULL for sending file size, could not send\n");
        return -1;
    }

    // set all values in protocol
    int sock_fd = sock->client_sock_fd;
    commands command = sock->command;
    char* first_filepath = strdup(sock->first_filepath);
    char* sec_filepath = NULL;
    if (sock->sec_filepath != NULL && command != RM) { // second value is omitted then deal with here for server
        sec_filepath = strdup(sock->sec_filepath);
        if (!sec_filepath) {
            perror("ERROR: strdup failed for second filepath, could not send");
            return -1;
        }
    } else {
        sec_filepath = (char*)calloc(1, 1); // allocate 1 byte for null terminator
        if (!sec_filepath) {
            perror("ERROR: calloc failed for second filepath, could not send");
            return -1;
        }
        sec_filepath[0] = '\0';
    }

    // serialize
    uint32_t fpath1_len = strlen(first_filepath);
    uint32_t fpath2_len = strlen(sec_filepath);
    

    struct header h;
    h.command = htonl(command);
    h.fpath1_len = htonl(fpath1_len);
    h.fpath2_len = htonl(fpath2_len);
    h.file_size = htonl(sock->file_size); // stored as unit32_t in socket_md_t, still need htonl since sending over network

    /* Send header */
    if (send(sock_fd, &h, sizeof(h), 0) != sizeof(h)) {
        perror("ERROR: send header could not be sent");
        return -1;
    }

    /* Send filenames */
    send(sock_fd, first_filepath, fpath1_len, 0);
    send(sock_fd, sec_filepath, fpath2_len, 0);

    free(first_filepath);
    free(sec_filepath);

    return 0;
}


/*
send_file
*/
ssize_t send_file(const char* first_filepath, int sock_fd) {
    if (first_filepath == NULL) {
        fprintf(stderr, "ERROR: file send - read filename is NULL, could not send\n");
        return -1;
    }
    if (sock_fd < 0) {
        fprintf(stderr, "ERROR: file send - socket file descriptor is invalid, could not send\n");
        return -1;
    }

    // Lock the socket mutex to ensure thread-safety when working with the file and socket
    pthread_mutex_lock(&send_mutex);
    // printf("Local File path: %s\n", file_path);
    FILE *file = fopen(first_filepath, "rb"); // "rb" for read binary
    if (file == NULL) {
        fprintf(stderr, "ERROR: file send - Issue opening read file, could not send\n");
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
                fprintf(stderr, "ERROR: file send - Unable to send file, could not send\n");
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
