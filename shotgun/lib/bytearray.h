OBJECT bytearray_new(STATE, int sz);
void *bytearray_byte_address(STATE, OBJECT self);
int bytearray_bytes(STATE, OBJECT self);
char *bytearray_as_string(STATE, OBJECT self);

#define BYTEARRAY_ADDRESS(self) ((void*)(self + (HEADER_SIZE * REFSIZE)))
#define BYTEARRAY_SIZE(self) SIZE_OF_BODY(self)

#define bytearray_byte_address(st, self) BYTEARRAY_ADDRESS(self)
#define bytearray_bytes(st, self) BYTEARRAY_SIZE(self)
