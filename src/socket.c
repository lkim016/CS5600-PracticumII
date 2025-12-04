/**
 * @file socket.c / source code for Socket.
 * @authors Lori Kim / CS5600 / Northeastern University
 * @brief
 * @date Nov 11, 2025 / Fall 2025
 *
 */

#include "socket.h"

ssize_t send_all(int sock, const void *buf, size_t len) {
    size_t sent = 0;
    const char *p = buf;
    while (sent < len) {
        ssize_t n = send(sock, p + sent, len - sent, 0);
        if (n <= 0) return n;
        sent += n;
    }
    return sent;
}


ssize_t rcv_all(int sock, void *buf, size_t len) {
    size_t recvd = 0;
    char *p = buf;
    while (recvd < len) {
        ssize_t n = recv(sock, p + recvd, len - recvd, 0);
        if (n <= 0) return n;
        recvd += n;
    }
    return recvd;
}

// SENDER
int send_msg(int sockfd, const char* command, const char* filename, const uint8_t* file_data, size_t file_size) {
    msg_header_t header;

    header.command_len = htonl(strlen(command));
    header.filename_len = htonl(strlen(filename));
    header.file_size = htonll(file_size);
    
    // Send header
    if (send_all(sockfd, &header, sizeof(header)) < 0)
        return -1;
    
    // Send command
    if (send_all(sockfd, command, strlen(command)) < 0)
        return -1;
    
    // Send filename
    if (send_all(sockfd, filename, strlen(filename)) < 0)
        return -1;
    
    // Send file data
    if (send_all(sockfd, file_data, file_size) < 0)
        return -1;
    
    return 0;
}

// RECEIVER
int rcv_msg(int sockfd, char** command, char** filename, uint8_t** file_data, size_t* file_size) {
    msg_header_t header;
    
    // Receive header
    if (rcv_all(sockfd, &header, sizeof(header)) < 0)
        return -1;
    
    // Convert from network byte order
    uint32_t cmd_len = ntohl(header.command_len);
    uint32_t fname_len = ntohl(header.filename_len);
    *file_size = ntohll(header.file_size);
    
    // Validate lengths (security!)
    if (cmd_len > 1024 || fname_len > 4096 || *file_size > 1ULL << 32) {
        return -1;  // Reject suspiciously large values
    }
    
    // Allocate and receive command
    *command = malloc(cmd_len + 1);
    if (recv_all(sockfd, *command, cmd_len) < 0) {
        free(*command);
        return -1;
    }
    (*command)[cmd_len] = '\0';
    
    // Allocate and receive filename
    *filename = malloc(fname_len + 1);
    if (recv_all(sockfd, *filename, fname_len) < 0) {
        free(*command);
        free(*filename);
        return -1;
    }
    (*filename)[fname_len] = '\0';
    
    // Allocate and receive file data
    *file_data = malloc(*file_size);
    if (recv_all(sockfd, *file_data, *file_size) < 0) {
        free(*command);
        free(*filename);
        free(*file_data);
        return -1;
    }
    
    return 0;
}