#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "breeze.h"
#include "buffer.h"

breeze_bytes_buf_t *
breeze_new_bytes_buf_from_bytes(const uint8_t *raw_bytes, size_t len, byte_order_t order, uint8_t read_only) {
    breeze_bytes_buf_t *bb = (breeze_bytes_buf_t *) malloc(sizeof(breeze_bytes_buf_t));
    if (bb == NULL)
    {
        die("Out of memory");
    }
    if (!read_only)
    {
        bb->buffer = (uint8_t *) malloc(len * sizeof(uint8_t));
        if (bb->buffer == NULL)
        {
            free(bb);
            die("Out of memory");
        }
        memcpy(bb->buffer, raw_bytes, len);
    } else {
        bb->buffer = (uint8_t *) raw_bytes;
    }
    bb->_read_only = read_only;
    bb->order = order;
    bb->read_pos = 0;
    bb->write_pos = len;
    bb->capacity = len;
    return bb;
}

breeze_bytes_buf_t *
breeze_new_bytes_buf(size_t capacity, byte_order_t order) {
    breeze_bytes_buf_t *bb = (breeze_bytes_buf_t *) malloc(sizeof(breeze_bytes_buf_t));
    if (bb == NULL)
    {
        die("Out of memory");
    }
    bb->buffer = (uint8_t *) malloc(capacity * sizeof(uint8_t));
    if (bb->buffer == NULL)
    {
        free(bb);
        die("Out of memory");
    }
    bb->_read_only = 0;
    bb->order = order;
    bb->read_pos = 0;
    bb->write_pos = 0;
    bb->capacity = capacity;
    return bb;
}

void breeze_free_bytes_buf(breeze_bytes_buf_t *bb) {
    if (bb == NULL)
    {
        return;
    }
    if (bb->buffer != NULL)
    {
        if (!bb->_read_only)
        {
            free(bb->buffer);
        }
        bb->buffer = NULL;
    }
    free(bb);
}

static void bb_grow_buf(breeze_bytes_buf_t *bb, size_t n) {
    assert(!bb->_read_only);
    size_t new_cap = 2 * bb->capacity + n;
    uint8_t *new_buf = (uint8_t *)malloc(new_cap * sizeof(uint8_t));
    if (new_buf == NULL)
    {
        die("Out of memery");
    }
    memcpy(new_buf, bb->buffer, bb->capacity);
    free(bb->buffer);
    bb->buffer = new_buf;
    bb->capacity = new_cap;
}

void bb_set_write_pos(breeze_bytes_buf_t *bb, uint32_t pos) {
    assert(!bb->_read_only);
    if (bb->capacity < pos)
    {
        bb_grow_buf(bb, pos - bb->capacity);
    }
    bb->write_pos = pos;
}

uint32_t bb_get_write_pos(breeze_bytes_buf_t *bb) {
    return bb->write_pos;
}

void bb_set_read_pos(breeze_bytes_buf_t *bb, uint32_t pos) {
    bb->read_pos = pos;
}

uint32_t bb_get_read_pos(breeze_bytes_buf_t *bb) {
    return bb->read_pos;
}

inline void bb_reset(breeze_bytes_buf_t *bb) {
    bb->read_pos = 0;
    bb->write_pos = 0;
}

inline int bb_remain(breeze_bytes_buf_t *bb) {
    return bb->write_pos - bb->read_pos;
}

void bb_write_bytes(breeze_bytes_buf_t *bb, const uint8_t *bytes, int len) {
    assert(!bb->_read_only);
    if (bb->capacity < bb->write_pos + len)
    {
        bb_grow_buf(bb, len);
    }
    // ? why add bb->write_pos
    memcpy((void *) (bb->buffer + bb->write_pos), (void *) bytes, len);
    bb->write_pos += len;
}

void bb_write_byte(breeze_bytes_buf_t *bb, uint8_t u) {
    assert(!bb->_read_only);
    if (bb->capacity < bb->write_pos + 1)
    {
        bb_grow_buf(bb, 1);
    }
    bb->buffer[bb->write_pos] = u;
    bb->write_pos++;
}

void bb_write_uint16(breeze_bytes_buf_t *bb, uint16_t u) {
    assert(!bb->_read_only);
    if (bb->capacity < bb->write_pos + 2)
    {
        bb_grow_buf(bb, 2);
    }
    if (bb->order == B_BIG_ENDIAN)
    {
        big_endian_write_uint16(bb->buffer + bb->write_pos, u);
    } else {
        little_endian_write_uint16(bb->buffer + bb->write_pos, u);
    }
    bb->write_pos += 2;
}

void bb_write_uint32(breeze_bytes_buf_t *bb, uint32_t u) {
    assert(!bb->_read_only);
    if (bb->capacity < bb->write_pos + 4)
    {
        bb_grow_buf(bb, 4);
    }
    if (bb->order == B_BIG_ENDIAN)
    {
        big_endian_write_uint32(bb->buffer + bb->write_pos, u);
    } else {
        little_endian_write_uint32(bb->buffer + bb->write_pos, u);
    }
    bb->write_pos += 4;
}

void bb_write_uint64(breeze_bytes_buf_t *bb, uint64_t u) {
    assert(!bb->_read_only);
    if (bb->capacity < bb->write_pos + 8)
    {
        bb_grow_buf(bb, 8);
    }
    if (bb->order == B_BIG_ENDIAN)
    {
        big_endian_write_uint64(bb->buffer + bb->write_pos, u);
    } else {
        little_endian_write_uint64(bb->buffer + bb->write_pos, u);
    }
    bb->write_pos += 8;
}

void bb_write_zigzag32(breeze_bytes_buf_t *bb, uint32_t u) {
    bb_write_varint(bb, (uint64_t)((u << 1) ^ (uint32_t)((int32_t)u>>31)));
}

void bb_write_zigzag64(breeze_bytes_buf_t *bb, uint64_t u) {
    bb_write_varint(bb, (uint64_t)((u << 1) ^ (uint64_t)((int64_t)u>>63)));
}

void bb_write_varint(breeze_bytes_buf_t *bb, uint64_t u) {
    assert(!bb->_read_only);
    int l = 0;
    for (; u >= 1 << 7;) {
        bb_write_byte(bb, (uint8_t) ((u & 0x7f) | 0x80));
        u >>= 7;
        l++;
    }
    bb_write_byte(bb, (uint8_t) u);
    l ++;
}

int bb_read_bytes(breeze_bytes_buf_t *bb, uint8_t *bs, int len) {
    if (bb_remain(bb) < len) {
        return E_BREEZE_BUFFER_NOT_ENOUGH;
    }
    memcpy((void *) bs, (void *) (bb->buffer + bb->read_pos), len);
    bb->read_pos += len;
    return BREEZE_OK;
}

int bb_read_byte(breeze_bytes_buf_t *bb, uint8_t *u) {
    if (bb_remain(bb) < 1) {
        return E_BREEZE_BUFFER_NOT_ENOUGH;
    }
    *u = bb->buffer[bb->read_pos];
    bb->read_pos++;
    return BREEZE_OK;
}

int bb_read_uint16(breeze_bytes_buf_t *bb, uint16_t *u) {
    if (bb_remain(bb) < 2) {
        return E_BREEZE_BUFFER_NOT_ENOUGH;
    }
    if (bb->order == B_BIG_ENDIAN) {
        *u = big_endian_read_uint16(bb->buffer + bb->read_pos);
    } else {
        *u = little_endian_read_uint16(bb->buffer + bb->read_pos);
    }
    bb->read_pos += 2;
    return BREEZE_OK;
}

int bb_read_uint32(breeze_bytes_buf_t *bb, uint32_t *u) {
    if (bb_remain(bb) < 4) {
        return E_BREEZE_BUFFER_NOT_ENOUGH;
    }
    if (bb->order == B_BIG_ENDIAN) {
        *u = big_endian_read_uint32(bb->buffer + bb->read_pos);
    } else {
        *u = little_endian_read_uint32(bb->buffer + bb->read_pos);
    }
    bb->read_pos += 4;
    return BREEZE_OK;
}

int bb_read_uint64(breeze_bytes_buf_t *bb, uint64_t *u) {
    if (bb_remain(bb) < 8) {
        return E_BREEZE_BUFFER_NOT_ENOUGH;
    }
    if (bb->order == B_BIG_ENDIAN) {
        *u = big_endian_read_uint64(bb->buffer + bb->read_pos);
    } else {
        *u = little_endian_read_uint64(bb->buffer + bb->read_pos);
    }
    bb->read_pos += 8;
    return BREEZE_OK;
}

int bb_read_zigzag32(breeze_bytes_buf_t *bb, uint64_t *v) {
    uint64_t u;
    int err;
    err = bb_read_varint(bb, &u);
    if(err != BREEZE_OK) {
        return err;
    }
    u = (uint64_t)((uint32_t)u >> 1) ^ (uint32_t)(-(int32_t)(u & 1));
    *v = u;
    return 0;
}

int bb_read_zigzag64(breeze_bytes_buf_t *bb, uint64_t *v) {
    uint64_t u;
    int err;
    err = bb_read_varint(bb, &u);
    if(err != BREEZE_OK) {
        return err;
    }
    u = (u >> 1) ^ (uint64_t)(-(int64_t)(u & 1));
    *v = u;
    return 0;
}

int bb_read_varint(breeze_bytes_buf_t *bb, uint64_t *u) {
    uint64_t r = 0;
    for (int shift = 0; shift < 64; shift += 7) {
        uint8_t b;
        int err = bb_read_byte(bb, &b);
        if (err != BREEZE_OK) {
            return err;
        }
        if ((b & 0x80) != 0x80) {
            r |= (uint64_t) b << shift;
            *u = r;
            return BREEZE_OK;
        }
        r |= (uint64_t) (b & 0x7f) << shift;
    }
    return E_BREEZE_OVERFLOW;
}

char *itoa(u_int64_t value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36)
    {
        *result = '\0';
        return result;
    }

    char *ptr = result, *ptr1 = result, tmp_char;
    u_int64_t tmp_value;

    do
    {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign
    // if (tmp_value < 0)
    // 	*ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}
