local ffi = require "ffi"
local C = ffi.C
local breeze = ffi.load("breeze")

ffi.cdef([[
char *itoa(uint64_t value, char *result, int base);
const char *breeze_error(int err);
typedef enum {
BREEZE_OK = 0,
E_BREEZE_BUFFER_NOT_ENOUGH  = -1,
E_BREEZE_OVERFLOW  = -2,
E_BREEZE_UNSUPPORTED_TYPE  = -3,
E_BREEZE_MEMORY_NOT_ENOUGH  = -4,
E_BREEZE_WRONG_SIZE  = -5
} breeze_error_code_t;
]])

local big_num = 9223372036854775808
print(big_num)
local str_num = ffi.new('char[?]', 100)
breeze.itoa(big_num, str_num, 10)
print(ffi.string(str_num))

local brz_ok_msg = breeze.breeze_error(C.E_BREEZE_BUFFER_NOT_ENOUGH)
print(ffi.string(brz_ok_msg))
print('--->', brz_ok_msg)

