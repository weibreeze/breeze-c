#ifndef BREEZE_LUA_BUFFER_H
#define BREEZE_LUA_BUFFER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "breeze.h"

//typedef struct{
//  int age;
//  const char *name;
//} user_t;
//
//typedef struct{
//  user_t *user;
//};

typedef enum {
    B_BIG_ENDIAN,
    B_LITTLE_ENDIAN
} byte_order_t;

//typedef struct {
//    int index;
//    const char *name;
//} msg_type_ref_t;
//
//typedef struct {
//  int msg_type_ref_count;
//  msg_type_ref_t *msg_type_ref;
//} breeze_buf_ctx_t;
//
//extern const char *get_msg_type_name(int index);
//extern int get_msg_type_index(const char *name);
//extern void put_msg_type(const char *name);

typedef void* breeze_buf_ctx_t;

typedef struct {
    uint8_t *buffer;
    byte_order_t order;
    uint32_t write_pos;
    uint32_t read_pos;
    size_t capacity;
    uint8_t _read_only;
    breeze_buf_ctx_t *bb_ctx;
} breeze_bytes_buf_t;

__unused static void die(const char *fmt, ...) {
    va_list arg;

    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
    fprintf(stderr, "\n");
    exit(-1);
}


__unused static void dump_bytes(const uint8_t *bs, int len) {
    for (int i = 0; i < len; i++) {
        printf("%02x", (uint8_t) bs[i]);
    }
    printf("\n");
}

__unused static void bb_dump(breeze_bytes_buf_t *bb) {
    fprintf(stderr, "bb: %p,", bb);
    fprintf(stderr, "read_only: %s,", bb->_read_only ? "true" : "false");
    fprintf(stderr, "write_pos: %d,", bb->write_pos);
    fprintf(stderr, "read_post: %d,", bb->read_pos);
    fprintf(stderr, "capacity: %zu,", bb->capacity);
    fprintf(stderr, "buffer: %p\n", bb->buffer);
}

extern breeze_bytes_buf_t *
breeze_new_bytes_buffer(size_t capacity, byte_order_t order);

extern breeze_bytes_buf_t *
breeze_new_bytes_buffer_from_bytes(const uint8_t *raw_bytes, size_t size, byte_order_t order, uint8_t read_only);

extern void breeze_free_bytes_buffer(breeze_bytes_buf_t *bb);

extern void bb_write_bytes(breeze_bytes_buf_t *bb, const uint8_t *bytes, int len);

extern void bb_write_byte(breeze_bytes_buf_t *bb, uint8_t u);

extern void bb_write_uint16(breeze_bytes_buf_t *bb, uint16_t u);

extern void bb_write_uint32(breeze_bytes_buf_t *bb, uint32_t u);

extern void bb_write_uint64(breeze_bytes_buf_t *bb, uint64_t u);

extern void bb_write_zigzag32(breeze_bytes_buf_t *bb, uint32_t u);

extern void bb_write_zigzag64(breeze_bytes_buf_t *bb, uint64_t u);

extern void bb_write_varint(breeze_bytes_buf_t *bb, uint64_t u);

extern void bb_set_write_pos(breeze_bytes_buf_t *bb, uint32_t pos);

extern uint32_t bb_get_write_pos(breeze_bytes_buf_t *bb);

extern void bb_set_read_pos(breeze_bytes_buf_t *bb, uint32_t pos);

extern uint32_t bb_get_read_pos(breeze_bytes_buf_t *bb);

extern int bb_remain(breeze_bytes_buf_t *bb);

extern void bb_reset(breeze_bytes_buf_t *bb);

extern int bb_read_bytes(breeze_bytes_buf_t *bb, uint8_t *bs, int len);

extern int bb_read_byte(breeze_bytes_buf_t *bb, uint8_t *u);

extern int bb_read_uint16(breeze_bytes_buf_t *bb, uint16_t *u);

extern int bb_read_uint32(breeze_bytes_buf_t *bb, uint32_t *u);

extern int bb_read_uint64(breeze_bytes_buf_t *bb, uint64_t *u);

extern int bb_read_zigzag32(breeze_bytes_buf_t *bb, uint64_t *u);

extern int bb_read_zigzag64(breeze_bytes_buf_t *bb, uint64_t *u);

extern int bb_read_varint(breeze_bytes_buf_t *bb, uint64_t *u);

extern char *itoa(u_int64_t value, char *result, int base);

//extern breeze_buf_ctx_t *get_ctx(breeze_bytes_buf_t *bb);

static inline uint64_t zigzag_encode(int64_t u) {
    return (u << 1) ^ (u >> 63);
}

static inline int64_t zigzag_decode(uint64_t n) {
    return (n >> 1) ^ -((int64_t) n & 1);
}

// 高位低址
static inline void big_endian_write_uint16(uint8_t *buffer, uint16_t n) {
    buffer[0] = (uint8_t) (n >> 8);
    buffer[1] = (uint8_t) n;

}

static inline void big_endian_write_uint32(uint8_t *buffer, uint32_t n) {
    buffer[0] = (uint8_t) (n >> 24);
    buffer[1] = (uint8_t) (n >> 16);
    buffer[2] = (uint8_t) (n >> 8);
    buffer[3] = (uint8_t) n;
}

static inline void big_endian_write_uint64(uint8_t *buffer, uint64_t n) {
    buffer[0] = (uint8_t) (n >> 56);
    buffer[1] = (uint8_t) (n >> 48);
    buffer[2] = (uint8_t) (n >> 40);
    buffer[3] = (uint8_t) (n >> 32);
    buffer[4] = (uint8_t) (n >> 24);
    buffer[5] = (uint8_t) (n >> 16);
    buffer[6] = (uint8_t) (n >> 8);
    buffer[7] = (uint8_t) n;
}

static inline uint16_t big_endian_read_uint16(uint8_t *buffer) {
    return ((uint16_t) buffer[0] << 8) | (uint16_t) buffer[1];
}

static inline uint32_t big_endian_read_uint32(uint8_t *buffer) {
    return ((uint32_t) buffer[0] << 24)
           | ((uint32_t) buffer[1] << 16)
           | ((uint32_t) buffer[2] << 8)
           | (uint32_t) buffer[3];
}

static inline uint64_t big_endian_read_uint64(uint8_t *buffer) {
    return ((uint64_t) buffer[0] << 56)
           | ((uint64_t) buffer[1] << 48)
           | ((uint64_t) buffer[2] << 40)
           | ((uint64_t) buffer[3] << 32)
           | ((uint64_t) buffer[4] << 24)
           | ((uint64_t) buffer[5] << 16)
           | ((uint64_t) buffer[6] << 8)
           | (uint64_t) buffer[7];

}

// 高位高址
static inline void little_endian_write_uint16(uint8_t *buffer, uint16_t n) {
    buffer[1] = (uint8_t) (n >> 8);
    buffer[0] = (uint8_t) n;

}

static inline void little_endian_write_uint32(uint8_t *buffer, uint32_t n) {
    buffer[3] = (uint8_t) (n >> 24);
    buffer[2] = (uint8_t) (n >> 16);
    buffer[1] = (uint8_t) (n >> 8);
    buffer[0] = (uint8_t) n;
}

static inline void little_endian_write_uint64(uint8_t *buffer, uint64_t n) {
    buffer[7] = (uint8_t) (n >> 56);
    buffer[6] = (uint8_t) (n >> 48);
    buffer[5] = (uint8_t) (n >> 40);
    buffer[4] = (uint8_t) (n >> 32);
    buffer[3] = (uint8_t) (n >> 24);
    buffer[2] = (uint8_t) (n >> 16);
    buffer[1] = (uint8_t) (n >> 8);
    buffer[0] = (uint8_t) n;
}

static inline uint16_t little_endian_read_uint16(uint8_t *buffer) {
    return ((uint16_t) buffer[1] << 8) | (uint16_t) buffer[0];
}

static inline uint32_t little_endian_read_uint32(uint8_t *buffer) {
    return ((uint32_t) buffer[3] << 24)
           | ((uint32_t) buffer[2] << 16)
           | ((uint32_t) buffer[1] << 8)
           | (uint32_t) buffer[0];
}

static inline uint64_t little_endian_read_uint64(uint8_t *buffer) {
    return ((uint64_t) buffer[7] << 56)
           | ((uint64_t) buffer[6] << 48)
           | ((uint64_t) buffer[5] << 40)
           | ((uint64_t) buffer[4] << 32)
           | ((uint64_t) buffer[3] << 24)
           | ((uint64_t) buffer[2] << 16)
           | ((uint64_t) buffer[1] << 8)
           | (uint64_t) buffer[0];

}

#endif //BREEZE_LUA_BUFFER_H
