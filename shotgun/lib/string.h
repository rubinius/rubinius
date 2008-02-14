#ifndef RBS_STRING_H
#define RBS_STRING_H

#include <ctype.h>

// copied from ruby 1.8.x source, ruby.h
/* need to include <ctype.h> to use these macros */
#ifndef ISPRINT
#define ISASCII(c) isascii((int)(unsigned char)(c))
#undef ISPRINT
#define ISPRINT(c) (ISASCII(c) && isprint((int)(unsigned char)(c)))
#define ISSPACE(c) (ISASCII(c) && isspace((int)(unsigned char)(c)))
#define ISUPPER(c) (ISASCII(c) && isupper((int)(unsigned char)(c)))
#define ISLOWER(c) (ISASCII(c) && islower((int)(unsigned char)(c)))
#define ISALNUM(c) (ISASCII(c) && isalnum((int)(unsigned char)(c)))
#define ISALPHA(c) (ISASCII(c) && isalpha((int)(unsigned char)(c)))
#define ISDIGIT(c) (ISASCII(c) && isdigit((int)(unsigned char)(c)))
#define ISXDIGIT(c) (ISASCII(c) && isxdigit((int)(unsigned char)(c)))
#endif

OBJECT string_new(STATE, const char *str);
OBJECT string_new2(STATE, const char *str, int sz);
OBJECT string_dup(STATE, OBJECT self);
OBJECT string_append(STATE, OBJECT self, OBJECT other);
char *string_byte_address(STATE, OBJECT self);
unsigned int string_hash_int(STATE, OBJECT self);
unsigned int string_hash_cstr(STATE, const char *bp);
unsigned int string_hash_str_with_size(STATE, const char *bp, int size);
OBJECT string_to_sym(STATE, OBJECT self);
double string_to_double(STATE, OBJECT self);
unsigned int string_hash_str(unsigned char *bp, unsigned int sz);
OBJECT string_newfrombstr(STATE, bstring output);
int string_equal_p(STATE, OBJECT self, OBJECT other);

#define string_unshare(state, cur) \
if(string_get_shared(cur) == Qtrue) { string_set_data(cur, bytearray_dup(state, string_get_data(cur))); string_set_shared(cur, Qnil); }

#endif
