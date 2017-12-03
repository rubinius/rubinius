/**********************************************************************

  transcode_data.h -

  $Author: nobu $
  created at: Mon 10 Dec 2007 14:01:47 JST 2007

  Copyright (C) 2007 Martin Duerst

**********************************************************************/

#ifndef RUBY_TRANSCODE_DATA_H
#define RUBY_TRANSCODE_DATA_H 1

#ifdef __cplusplus
extern "C" {
#endif

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define WORDINDEX_SHIFT_BITS 2
#define WORDINDEX2INFO(widx)      ((widx) << WORDINDEX_SHIFT_BITS)
#define INFO2WORDINDEX(info)      ((info) >> WORDINDEX_SHIFT_BITS)
#define BYTE_LOOKUP_BASE(bl) ((bl)[0])
#define BYTE_LOOKUP_INFO(bl) ((bl)[1])

#define PType (unsigned int)

#define NOMAP	(PType 0x01)	/* direct map */
#define ONEbt	(0x02)		/* one byte payload */
#define TWObt	(0x03)		/* two bytes payload */
#define THREEbt	(0x05)		/* three bytes payload */
#define FOURbt	(0x06)		/* four bytes payload, UTF-8 only, macros start at getBT0 */
#define INVALID	(PType 0x07)	/* invalid byte sequence */
#define UNDEF	(PType 0x09)	/* legal but undefined */
#define ZERObt	(PType 0x0A)	/* zero bytes of payload, i.e. remove */
#define FUNii	(PType 0x0B)	/* function from info to info */
#define FUNsi	(PType 0x0D)	/* function from start to info */
#define FUNio	(PType 0x0E)	/* function from info to output */
#define FUNso	(PType 0x0F)	/* function from start to output */
#define STR1	(PType 0x11)	/* string 4 <= len <= 259 bytes: 1byte length + content */
#define GB4bt	(PType 0x12)	/* GB18030 four bytes payload */
#define FUNsio  (PType 0x13)    /* function from start and info to output */

#define STR1_LENGTH(byte_addr) (unsigned int)(*(byte_addr) + 4)
#define STR1_BYTEINDEX(w) ((w) >> 6)
#define makeSTR1(bi) (((bi) << 6) | STR1)
#define makeSTR1LEN(len) ((len)-4)

#define o1(b1)		(PType((((unsigned char)(b1))<<8)|ONEbt))
#define o2(b1,b2)	(PType((((unsigned char)(b1))<<8)|(((unsigned char)(b2))<<16)|TWObt))
#define o3(b1,b2,b3)	(PType(((((unsigned char)(b1))<<8)|(((unsigned char)(b2))<<16)|(((unsigned int)(unsigned char)(b3))<<24)|THREEbt)&0xffffffffU))
#define o4(b0,b1,b2,b3)	(PType(((((unsigned char)(b1))<<8)|(((unsigned char)(b2))<<16)|(((unsigned char)(b3))<<24)|((((unsigned char)(b0))&0x07)<<5)|FOURbt)&0xffffffffU))
#define g4(b0,b1,b2,b3) (PType(((((unsigned char)(b0))<<8)|(((unsigned char)(b2))<<16)|((((unsigned char)(b1))&0x0f)<<24)|((((unsigned int)(unsigned char)(b3))&0x0f)<<28)|GB4bt)&0xffffffffU))
#define funsio(diff)	(PType((((unsigned int)(diff))<<8)|FUNsio))

#define getBT1(a)	((unsigned char)((a)>> 8))
#define getBT2(a)	((unsigned char)((a)>>16))
#define getBT3(a)	((unsigned char)((a)>>24))
#define getBT0(a)	(((unsigned char)((a)>> 5)&0x07)|0xF0)   /* for UTF-8 only!!! */

#define getGB4bt0(a)	((unsigned char)((a)>> 8))
#define getGB4bt1(a)	(((unsigned char)((a)>>24)&0x0F)|0x30)
#define getGB4bt2(a)	((unsigned char)((a)>>16))
#define getGB4bt3(a)	(((unsigned char)((a)>>28)&0x0F)|0x30)

#define o2FUNii(b1,b2)	(PType((((unsigned char)(b1))<<8)|(((unsigned char)(b2))<<16)|FUNii))

/* do we need these??? maybe not, can be done with simple tables */
#define ONETRAIL       /* legal but undefined if one more trailing UTF-8 */
#define TWOTRAIL       /* legal but undefined if two more trailing UTF-8 */
#define THREETRAIL     /* legal but undefined if three more trailing UTF-8 */

#define ECONV_ERROR_HANDLER_MASK                0x000000ff

#define ECONV_INVALID_MASK                      0x0000000f
#define ECONV_INVALID_REPLACE                   0x00000002

#define ECONV_UNDEF_MASK                        0x000000f0
#define ECONV_UNDEF_REPLACE                     0x00000020
#define ECONV_UNDEF_HEX_CHARREF                 0x00000030

#define ECONV_DECORATOR_MASK                    0x0000ff00
#define ECONV_NEWLINE_DECORATOR_MASK            0x00003f00
#define ECONV_NEWLINE_DECORATOR_READ_MASK       0x00000f00
#define ECONV_NEWLINE_DECORATOR_WRITE_MASK      0x00003000

#define ECONV_UNIVERSAL_NEWLINE_DECORATOR       0x00000100
#define ECONV_CRLF_NEWLINE_DECORATOR            0x00001000
#define ECONV_CR_NEWLINE_DECORATOR              0x00002000
#define ECONV_XML_TEXT_DECORATOR                0x00004000
#define ECONV_XML_ATTR_CONTENT_DECORATOR        0x00008000

#define ECONV_STATEFUL_DECORATOR_MASK           0x00f00000
#define ECONV_XML_ATTR_QUOTE_DECORATOR          0x00100000

#if defined(_WIN32)
#define ECONV_DEFAULT_NEWLINE_DECORATOR CONVERTER_CRLF_NEWLINE_DECORATOR
#else
#define ECONV_DEFAULT_NEWLINE_DECORATOR 0
#endif

#define ECONV_PARTIAL_INPUT                     0x00010000
#define ECONV_AFTER_OUTPUT                      0x00020000

typedef enum {
  econv_invalid_byte_sequence,
  econv_undefined_conversion,
  econv_destination_buffer_full,
  econv_source_buffer_empty,
  econv_finished,
  econv_after_output,
  econv_incomplete_input
} rb_econv_result_t;

typedef enum {
  asciicompat_converter,        /* ASCII-compatible -> ASCII-compatible */
  asciicompat_decoder,          /* ASCII-incompatible -> ASCII-compatible */
  asciicompat_encoder           /* ASCII-compatible -> ASCII-incompatible */
  /* ASCII-incompatible -> ASCII-incompatible is intentionally omitted. */
} rb_transcoder_asciicompat_type_t;

/* static structure, one per supported encoding pair */
struct rb_transcoder {
    const char *src_encoding;
    const char *dst_encoding;
    unsigned int conv_tree_start;
    const unsigned char *byte_array;
    unsigned int byte_array_length;
    const unsigned int *word_array;
    unsigned int word_array_length;
    int word_size;
    int input_unit_length;
    int max_input;
    int max_output;
    rb_transcoder_asciicompat_type_t asciicompat_type;
    size_t state_size;
    int (*state_init_func)(void*); /* ret==0:success ret!=0:failure(errno) */
    int (*state_fini_func)(void*); /* ret==0:success ret!=0:failure(errno) */
    unsigned int (*func_ii)(void*, unsigned int); /* info  -> info   */
    unsigned int (*func_si)(void*, const unsigned char*, size_t); /* start -> info   */
    ssize_t (*func_io)(void*, unsigned int, const unsigned char*, size_t); /* info  -> output */
    ssize_t (*func_so)(void*, const unsigned char*, size_t, unsigned char*, size_t); /* start -> output */
    ssize_t (*finish_func)(void*, unsigned char*, size_t); /* -> output */
    ssize_t (*resetsize_func)(void*); /* -> len */
    ssize_t (*resetstate_func)(void*, unsigned char*, size_t); /* -> output */
    ssize_t (*func_sio)(void*, const unsigned char*, size_t, unsigned int, unsigned char*, size_t); /* start -> output */
};

// TODO: remove
#ifndef VALUE
typedef intptr_t VALUE;
#endif

typedef struct {
    struct rb_transcoding *tc;
    unsigned char *out_buf_start;
    unsigned char *out_data_start;
    unsigned char *out_data_end;
    unsigned char *out_buf_end;
    rb_econv_result_t last_result;
} rb_econv_elem_t;

typedef struct {
  const char* destination_encoding_name;
  int num_transcoders;
  struct rb_transcoder** transcoders;
} rb_econv_replacement_converters;

struct rb_econv_t {
    int flags;
    const char *source_encoding_name;
    const char *destination_encoding_name;

    int started;

    const unsigned char *replacement_str;
    size_t replacement_len;
    const char *replacement_enc;
    int replacement_allocated;

    int num_replacement_converters;
    rb_econv_replacement_converters* replacement_converters;

    unsigned char *in_buf_start;
    unsigned char *in_data_start;
    unsigned char *in_data_end;
    unsigned char *in_buf_end;
    rb_econv_elem_t *elems;
    int num_allocated;
    int num_trans;
    int num_finished;
    struct rb_transcoding *last_tc;

    /* last error */
    struct {
        rb_econv_result_t result;
        struct rb_transcoding *error_tc;
        const char *source_encoding;
        const char *destination_encoding;
        const unsigned char *error_bytes_start;
        size_t error_bytes_len;
        size_t readagain_len;
    } last_error;
};

typedef struct rb_econv_t rb_econv_t;
typedef struct rb_transcoder rb_transcoder;
typedef struct rb_transcoder OnigTranscodingType;

void rb_declare_transcoder(const char *enc1, const char *enc2, const char *lib);
void rb_register_transcoder(const rb_transcoder *);

rb_econv_t* rb_econv_alloc(int n_hint);
void rb_econv_alloc_replacement_converters(rb_econv_t* ec, int num);
void rb_econv_free(rb_econv_t *ec);

rb_econv_result_t econv_convert(rb_econv_t *ec,
                                const unsigned char **input_ptr,
                                const unsigned char *input_stop,
                                unsigned char **output_ptr,
                                unsigned char *output_stop,
                                int flags);
int rb_econv_putbackable(rb_econv_t *ec);
void rb_econv_putback(rb_econv_t *ec, unsigned char *p, int n);

int rb_econv_add_transcoder_at(rb_econv_t *ec, const rb_transcoder *tr, int i);

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

/*
 * To get rid of collision of initializer symbols in statically-linked encodings
 * and transcoders
 */
#if defined(EXTSTATIC) && EXTSTATIC
# define TRANS_INIT(name) void Init_trans_ ## name(void)
#else
# define TRANS_INIT(name) void Init_ ## name(void)
#endif

#ifdef __cplusplus
}
#endif

#endif /* RUBY_TRANSCODE_DATA_H */
