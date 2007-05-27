#ifndef RBS_STRING
#define RBS_STRING 1

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
char *string_as_string(STATE, OBJECT self);
char *string_byte_address(STATE, OBJECT self);
unsigned int string_hash_int(STATE, OBJECT self);
unsigned int string_hash_cstr(STATE, const char *bp);
OBJECT string_to_sym(STATE, OBJECT self);
double string_to_double(STATE, OBJECT self);

#endif
