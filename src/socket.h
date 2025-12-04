/**
 * @file socket.h / header file for socket.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Nov 11, 2025 / Fall 2025
 *
 */


#pragma once
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>
#include "config.h"

#ifndef CACHE_H
#define CACHE_H

// This is the FIXED portion of the header.
// filename is *not* inside this struct (since it's variable-length).
// compiler might insert padding, making sender/receiver read different byte counts.
#pragma pack(push, 1)
typedef struct msg_header {
    uint32_t command_len;   // command length (big-endian)
    uint32_t filename_len;  // filename length (big-endian)
    uint64_t file_size;     // file size in bytes (big-endian)
} msg_header_t;
#pragma pack(pop)

ssize_t send_all(int sock, const void *buf, size_t len);

ssize_t rcv_all(int sock, void *buf, size_t len);

int send_message(int sockfd, const char* command, const char* filename, const uint8_t* file_data, size_t file_size);

int rcv_message(int sockfd, char** command, char** filename, uint8_t** file_data, size_t* file_size);

#endif