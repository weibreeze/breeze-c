#include "breeze.h"

const char *breeze_error(int err) {
    switch (err) {
        case BREEZE_OK:
            return "ok";
        case E_BREEZE_BUFFER_NOT_ENOUGH:
            return "breeze buffer not enough";
        case E_BREEZE_OVERFLOW:
            return "breeze number overflow";
        case E_BREEZE_UNSUPPORTED_TYPE:
            return "breeze unsupported type";
        case E_BREEZE_MEMORY_NOT_ENOUGH:
            return "breeze memory not enough";
        case E_BREEZE_WRONG_SIZE:
            return "breeze wrong data size";
        default:
            return "unknown error";
    }
}