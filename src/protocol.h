
#include <stdint.h>

#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef enum { NULL_VAL = 0, WRITE, GET, RM, STOP } commands;

struct __attribute__((packed)) header {
    uint32_t command;
    uint32_t fpath1_len;
    uint32_t fpath2_len;
    uint32_t file_size;
};

#endif