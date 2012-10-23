/**********************************************************************

  transcode.c -

  $Author: naruse $
  created at: Tue Oct 30 16:10:22 JST 2007

  Copyright (C) 2007 Martin Duerst

**********************************************************************/

#include "oniguruma.h"
#include "transcoder.h"

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

void* XMALLOC(size_t bytes);
void  XFREE(void* ptr);
void* XREALLOC(void* ptr, size_t bytes);
void* XCALLOC(size_t items, size_t bytes);

#define ALLOC_N(type,n)         ((type*)XMALLOC((n)*sizeof(type)))
#define ALLOC(type)             ((type*)XMALLOC(sizeof(type)))

#define REALLOC_N(var,type,n)   ((var)=(type*)XREALLOC((char*)(var),(n)*sizeof(type)))
#define ALLOCA_N(type,n)        ((type*)alloca(sizeof(type)*(n)))

#undef xmalloc
#undef xrealloc
#undef xcalloc
#undef xfree

#define xmalloc   XMALLOC
#define xcalloc   XCALLOC
#define xrealloc  XREALLOC
#define xfree     XFREE

#define MEMZERO(p,type,n)       memset((p), 0, sizeof(type)*(n))
#define MEMCPY(p1,p2,type,n)    memcpy((p1), (p2), sizeof(type)*(n))
#define MEMMOVE(p1,p2,type,n)   memmove((p1), (p2), sizeof(type)*(n))
#define MEMCMP(p1,p2,type,n)    memcmp((p1), (p2), sizeof(type)*(n))


#define ENABLE_ECONV_NEWLINE_OPTION 1

/* dynamic structure, one per conversion (similar to iconv_t) */
/* may carry conversion state (e.g. for iso-2022-jp) */
typedef struct rb_transcoding {
    const rb_transcoder *transcoder;

    int flags;

    int resume_position;
    unsigned int next_table;
    VALUE next_info;
    unsigned char next_byte;
    unsigned int output_index;

    ssize_t recognized_len; /* already interpreted */
    ssize_t readagain_len; /* not yet interpreted */
    union {
        unsigned char ary[8]; /* max_input <= sizeof(ary) */
        unsigned char *ptr; /* length: max_input */
    } readbuf; /* recognized_len + readagain_len used */

    ssize_t writebuf_off;
    ssize_t writebuf_len;
    union {
        unsigned char ary[8]; /* max_output <= sizeof(ary) */
        unsigned char *ptr; /* length: max_output */
    } writebuf;

    union rb_transcoding_state_t { /* opaque data for stateful encoding */
        void *ptr;
        char ary[sizeof(double) > sizeof(void*) ? sizeof(double) : sizeof(void*)];
        double dummy_for_alignment;
    } state;
} rb_transcoding;
#define TRANSCODING_READBUF(tc) \
    ((tc)->transcoder->max_input <= (int)sizeof((tc)->readbuf.ary) ? \
     (tc)->readbuf.ary : \
     (tc)->readbuf.ptr)
#define TRANSCODING_WRITEBUF(tc) \
    ((tc)->transcoder->max_output <= (int)sizeof((tc)->writebuf.ary) ? \
     (tc)->writebuf.ary : \
     (tc)->writebuf.ptr)
#define TRANSCODING_WRITEBUF_SIZE(tc) \
    ((tc)->transcoder->max_output <= (int)sizeof((tc)->writebuf.ary) ? \
     sizeof((tc)->writebuf.ary) : \
     (size_t)(tc)->transcoder->max_output)
#define TRANSCODING_STATE_EMBED_MAX ((int)sizeof(union rb_transcoding_state_t))
#define TRANSCODING_STATE(tc) \
    ((tc)->transcoder->state_size <= (int)sizeof((tc)->state) ? \
     (tc)->state.ary : \
     (tc)->state.ptr)

/*
 *  Dispatch data and logic
 */

#define DECORATOR_P(sname, dname) (*(sname) == '\0')

typedef struct {
    const char *sname;
    const char *dname;
    const char *lib; /* null means means no need to load a library */
    const rb_transcoder *transcoder;
} transcoder_entry_t;

#define encoding_equal(enc1, enc2) (strcasecmp((enc1), (enc2)) == 0)

/*
 *  Transcoding engine logic
 */

static const unsigned char *
transcode_char_start(rb_transcoding *tc,
                         const unsigned char *in_start,
                         const unsigned char *inchar_start,
                         const unsigned char *in_p,
                         size_t *char_len_ptr)
{
    const unsigned char *ptr;
    if (inchar_start - in_start < tc->recognized_len) {
        MEMCPY(TRANSCODING_READBUF(tc) + tc->recognized_len,
               inchar_start, unsigned char, in_p - inchar_start);
        ptr = TRANSCODING_READBUF(tc);
    }
    else {
        ptr = inchar_start - tc->recognized_len;
    }
    *char_len_ptr = tc->recognized_len + (in_p - inchar_start);
    return ptr;
}

static rb_econv_result_t
transcode_restartable0(const unsigned char **in_pos, unsigned char **out_pos,
                      const unsigned char *in_stop, unsigned char *out_stop,
                      rb_transcoding *tc,
                      const int opt)
{
    const rb_transcoder *tr = tc->transcoder;
    int unitlen = tr->input_unit_length;
    ssize_t readagain_len = 0;

    const unsigned char *inchar_start;
    const unsigned char *in_p;

    unsigned char *out_p;

    in_p = inchar_start = *in_pos;

    out_p = *out_pos;

#define SUSPEND(ret, num) \
    do { \
        tc->resume_position = (num); \
        if (0 < in_p - inchar_start) \
            MEMMOVE(TRANSCODING_READBUF(tc)+tc->recognized_len, \
                   inchar_start, unsigned char, in_p - inchar_start); \
        *in_pos = in_p; \
        *out_pos = out_p; \
        tc->recognized_len += in_p - inchar_start; \
        if (readagain_len) { \
            tc->recognized_len -= readagain_len; \
            tc->readagain_len = readagain_len; \
        } \
        return (ret); \
        resume_label ## num:; \
    } while (0)
#define SUSPEND_OBUF(num) \
    do { \
        while (out_stop - out_p < 1) { SUSPEND(econv_destination_buffer_full, num); } \
    } while (0)

#define SUSPEND_AFTER_OUTPUT(num) \
    if ((opt & ECONV_AFTER_OUTPUT) && *out_pos != out_p) { \
        SUSPEND(econv_after_output, num); \
    }

#define next_table (tc->next_table)
#define next_info (tc->next_info)
#define next_byte (tc->next_byte)
#define writebuf_len (tc->writebuf_len)
#define writebuf_off (tc->writebuf_off)

    switch (tc->resume_position) {
      case 0: break;
      case 1: goto resume_label1;
      case 2: goto resume_label2;
      case 3: goto resume_label3;
      case 4: goto resume_label4;
      case 5: goto resume_label5;
      case 6: goto resume_label6;
      case 7: goto resume_label7;
      case 8: goto resume_label8;
      case 9: goto resume_label9;
      case 10: goto resume_label10;
      case 11: goto resume_label11;
      case 12: goto resume_label12;
      case 13: goto resume_label13;
      case 14: goto resume_label14;
      case 15: goto resume_label15;
      case 16: goto resume_label16;
      case 17: goto resume_label17;
      case 18: goto resume_label18;
      case 19: goto resume_label19;
      case 20: goto resume_label20;
      case 21: goto resume_label21;
      case 22: goto resume_label22;
      case 23: goto resume_label23;
      case 24: goto resume_label24;
      case 25: goto resume_label25;
      case 26: goto resume_label26;
      case 27: goto resume_label27;
      case 28: goto resume_label28;
      case 29: goto resume_label29;
      case 30: goto resume_label30;
      case 31: goto resume_label31;
      case 32: goto resume_label32;
      case 33: goto resume_label33;
      case 34: goto resume_label34;
    }

    while (1) {
        inchar_start = in_p;
        tc->recognized_len = 0;
        next_table = tr->conv_tree_start;

        SUSPEND_AFTER_OUTPUT(24);

        if (in_stop <= in_p) {
            if (!(opt & ECONV_PARTIAL_INPUT))
                break;
            SUSPEND(econv_source_buffer_empty, 7);
            continue;
        }

#define BYTE_ADDR(index) (tr->byte_array + (index))
#define WORD_ADDR(index) (tr->word_array + INFO2WORDINDEX(index))
#define BL_BASE BYTE_ADDR(BYTE_LOOKUP_BASE(WORD_ADDR(next_table)))
#define BL_INFO WORD_ADDR(BYTE_LOOKUP_INFO(WORD_ADDR(next_table)))
#define BL_MIN_BYTE     (BL_BASE[0])
#define BL_MAX_BYTE     (BL_BASE[1])
#define BL_OFFSET(byte) (BL_BASE[2+(byte)-BL_MIN_BYTE])
#define BL_ACTION(byte) (BL_INFO[BL_OFFSET((byte))])

        next_byte = (unsigned char)*in_p++;
      follow_byte:
        if (next_byte < BL_MIN_BYTE || BL_MAX_BYTE < next_byte)
            next_info = INVALID;
        else {
            next_info = (VALUE)BL_ACTION(next_byte);
        }
      follow_info:
        switch (next_info & 0x1F) {
          case NOMAP:
            {
                const unsigned char *p = inchar_start;
                writebuf_off = 0;
                while (p < in_p) {
                    TRANSCODING_WRITEBUF(tc)[writebuf_off++] = (unsigned char)*p++;
                }
                writebuf_len = writebuf_off;
                writebuf_off = 0;
                while (writebuf_off < writebuf_len) {
                    SUSPEND_OBUF(3);
                    *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
                }
            }
            continue;
          case 0x00: case 0x04: case 0x08: case 0x0C:
          case 0x10: case 0x14: case 0x18: case 0x1C:
            SUSPEND_AFTER_OUTPUT(25);
            while (in_p >= in_stop) {
                if (!(opt & ECONV_PARTIAL_INPUT))
                    goto incomplete;
                SUSPEND(econv_source_buffer_empty, 5);
            }
            next_byte = (unsigned char)*in_p++;
            next_table = (unsigned int)next_info;
            goto follow_byte;
          case ZERObt: /* drop input */
            continue;
          case ONEbt:
            SUSPEND_OBUF(9); *out_p++ = getBT1(next_info);
            continue;
          case TWObt:
            SUSPEND_OBUF(10); *out_p++ = getBT1(next_info);
            SUSPEND_OBUF(21); *out_p++ = getBT2(next_info);
            continue;
          case THREEbt:
            SUSPEND_OBUF(11); *out_p++ = getBT1(next_info);
            SUSPEND_OBUF(15); *out_p++ = getBT2(next_info);
            SUSPEND_OBUF(16); *out_p++ = getBT3(next_info);
            continue;
          case FOURbt:
            SUSPEND_OBUF(12); *out_p++ = getBT0(next_info);
            SUSPEND_OBUF(17); *out_p++ = getBT1(next_info);
            SUSPEND_OBUF(18); *out_p++ = getBT2(next_info);
            SUSPEND_OBUF(19); *out_p++ = getBT3(next_info);
            continue;
          case GB4bt:
            SUSPEND_OBUF(29); *out_p++ = getGB4bt0(next_info);
            SUSPEND_OBUF(30); *out_p++ = getGB4bt1(next_info);
            SUSPEND_OBUF(31); *out_p++ = getGB4bt2(next_info);
            SUSPEND_OBUF(32); *out_p++ = getGB4bt3(next_info);
            continue;
          case STR1:
            tc->output_index = 0;
            while (tc->output_index < STR1_LENGTH(BYTE_ADDR(STR1_BYTEINDEX(next_info)))) {
                SUSPEND_OBUF(28); *out_p++ = BYTE_ADDR(STR1_BYTEINDEX(next_info))[1+tc->output_index];
                tc->output_index++;
            }
            continue;
          case FUNii:
            next_info = (VALUE)(*tr->func_ii)(TRANSCODING_STATE(tc), next_info);
            goto follow_info;
          case FUNsi:
            {
                const unsigned char *char_start;
                size_t char_len;
                char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                next_info = (VALUE)(*tr->func_si)(TRANSCODING_STATE(tc), char_start, (size_t)char_len);
                goto follow_info;
            }
          case FUNio:
            SUSPEND_OBUF(13);
            if (tr->max_output <= out_stop - out_p)
                out_p += tr->func_io(TRANSCODING_STATE(tc),
                    next_info, out_p, out_stop - out_p);
            else {
                writebuf_len = tr->func_io(TRANSCODING_STATE(tc),
                    next_info,
                    TRANSCODING_WRITEBUF(tc), TRANSCODING_WRITEBUF_SIZE(tc));
                writebuf_off = 0;
                while (writebuf_off < writebuf_len) {
                    SUSPEND_OBUF(20);
                    *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
                }
            }
            break;
          case FUNso:
            {
                const unsigned char *char_start;
                size_t char_len;
                SUSPEND_OBUF(14);
                if (tr->max_output <= out_stop - out_p) {
                    char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                    out_p += tr->func_so(TRANSCODING_STATE(tc),
                        char_start, (size_t)char_len,
                        out_p, out_stop - out_p);
                }
                else {
                    char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                    writebuf_len = tr->func_so(TRANSCODING_STATE(tc),
                        char_start, (size_t)char_len,
                        TRANSCODING_WRITEBUF(tc), TRANSCODING_WRITEBUF_SIZE(tc));
                    writebuf_off = 0;
                    while (writebuf_off < writebuf_len) {
                        SUSPEND_OBUF(22);
                        *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
                    }
                }
                break;
            }
      case FUNsio:
            {
                const unsigned char *char_start;
                size_t char_len;
                SUSPEND_OBUF(33);
                if (tr->max_output <= out_stop - out_p) {
                    char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                    out_p += tr->func_sio(TRANSCODING_STATE(tc),
                        char_start, (size_t)char_len, next_info,
                        out_p, out_stop - out_p);
                }
                else {
                    char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                    writebuf_len = tr->func_sio(TRANSCODING_STATE(tc),
                        char_start, (size_t)char_len, next_info,
                        TRANSCODING_WRITEBUF(tc), TRANSCODING_WRITEBUF_SIZE(tc));
                    writebuf_off = 0;
                    while (writebuf_off < writebuf_len) {
                        SUSPEND_OBUF(34);
                        *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
                    }
                }
                break;
            }
          case INVALID:
            if (tc->recognized_len + (in_p - inchar_start) <= unitlen) {
                if (tc->recognized_len + (in_p - inchar_start) < unitlen)
                    SUSPEND_AFTER_OUTPUT(26);
                while ((opt & ECONV_PARTIAL_INPUT) && tc->recognized_len + (in_stop - inchar_start) < unitlen) {
                    in_p = in_stop;
                    SUSPEND(econv_source_buffer_empty, 8);
                }
                if (tc->recognized_len + (in_stop - inchar_start) <= unitlen) {
                    in_p = in_stop;
                }
                else {
                    in_p = inchar_start + (unitlen - tc->recognized_len);
                }
            }
            else {
                ssize_t invalid_len; /* including the last byte which causes invalid */
                ssize_t discard_len;
                invalid_len = tc->recognized_len + (in_p - inchar_start);
                discard_len = ((invalid_len - 1) / unitlen) * unitlen;
                readagain_len = invalid_len - discard_len;
            }
            goto invalid;
          case UNDEF:
            goto undef;
          default:
      // TODO: remove
            // rb_raise(rb_eRuntimeError, "unknown transcoding instruction");
      return 0;
        }
        continue;

      invalid:
        SUSPEND(econv_invalid_byte_sequence, 1);
        continue;

      incomplete:
        SUSPEND(econv_incomplete_input, 27);
        continue;

      undef:
        SUSPEND(econv_undefined_conversion, 2);
        continue;
    }

    /* cleanup */
    if (tr->finish_func) {
        SUSPEND_OBUF(4);
        if (tr->max_output <= out_stop - out_p) {
            out_p += tr->finish_func(TRANSCODING_STATE(tc),
                out_p, out_stop - out_p);
        }
        else {
            writebuf_len = tr->finish_func(TRANSCODING_STATE(tc),
                TRANSCODING_WRITEBUF(tc), TRANSCODING_WRITEBUF_SIZE(tc));
            writebuf_off = 0;
            while (writebuf_off < writebuf_len) {
                SUSPEND_OBUF(23);
                *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
            }
        }
    }
    while (1)
        SUSPEND(econv_finished, 6);
#undef SUSPEND
#undef next_table
#undef next_info
#undef next_byte
#undef writebuf_len
#undef writebuf_off
}

static rb_econv_result_t
transcode_restartable(const unsigned char **in_pos, unsigned char **out_pos,
                      const unsigned char *in_stop, unsigned char *out_stop,
                      rb_transcoding *tc,
                      const int opt)
{
    if (tc->readagain_len) {
        unsigned char *readagain_buf = ALLOCA_N(unsigned char, tc->readagain_len);
        const unsigned char *readagain_pos = readagain_buf;
        const unsigned char *readagain_stop = readagain_buf + tc->readagain_len;
        rb_econv_result_t res;

        MEMCPY(readagain_buf, TRANSCODING_READBUF(tc) + tc->recognized_len,
               unsigned char, tc->readagain_len);
        tc->readagain_len = 0;
        res = transcode_restartable0(&readagain_pos, out_pos, readagain_stop, out_stop, tc, opt|ECONV_PARTIAL_INPUT);
        if (res != econv_source_buffer_empty) {
            MEMCPY(TRANSCODING_READBUF(tc) + tc->recognized_len + tc->readagain_len,
                   readagain_pos, unsigned char, readagain_stop - readagain_pos);
            tc->readagain_len += readagain_stop - readagain_pos;
            return res;
        }
    }
    return transcode_restartable0(in_pos, out_pos, in_stop, out_stop, tc, opt);
}

static rb_transcoding *
rb_transcoding_open_by_transcoder(const rb_transcoder *tr, int flags)
{
    rb_transcoding *tc;

    tc = ALLOC(rb_transcoding);
    tc->transcoder = tr;
    tc->flags = flags;
    if (TRANSCODING_STATE_EMBED_MAX < tr->state_size)
        tc->state.ptr = xmalloc(tr->state_size);
    if (tr->state_init_func) {
        (tr->state_init_func)(TRANSCODING_STATE(tc)); /* xxx: check return value */
    }
    tc->resume_position = 0;
    tc->recognized_len = 0;
    tc->readagain_len = 0;
    tc->writebuf_len = 0;
    tc->writebuf_off = 0;
    if ((int)sizeof(tc->readbuf.ary) < tr->max_input) {
        tc->readbuf.ptr = xmalloc(tr->max_input);
    }
    if ((int)sizeof(tc->writebuf.ary) < tr->max_output) {
        tc->writebuf.ptr = xmalloc(tr->max_output);
    }
    return tc;
}

static rb_econv_result_t
rb_transcoding_convert(rb_transcoding *tc,
  const unsigned char **input_ptr, const unsigned char *input_stop,
  unsigned char **output_ptr, unsigned char *output_stop,
  int flags)
{
    return transcode_restartable(
                input_ptr, output_ptr,
                input_stop, output_stop,
                tc, flags);
}

static void
rb_transcoding_close(rb_transcoding *tc)
{
    const rb_transcoder *tr = tc->transcoder;
    if (tr->state_fini_func) {
        (tr->state_fini_func)(TRANSCODING_STATE(tc)); /* check return value? */
    }
    if (TRANSCODING_STATE_EMBED_MAX < tr->state_size)
        xfree(tc->state.ptr);
    if ((int)sizeof(tc->readbuf.ary) < tr->max_input)
        xfree(tc->readbuf.ptr);
    if ((int)sizeof(tc->writebuf.ary) < tr->max_output)
        xfree(tc->writebuf.ptr);
    xfree(tc);
}

static size_t
rb_transcoding_memsize(rb_transcoding *tc)
{
    size_t size = sizeof(rb_transcoding);
    const rb_transcoder *tr = tc->transcoder;

    if (TRANSCODING_STATE_EMBED_MAX < tr->state_size) {
        size += tr->state_size;
    }
    if ((int)sizeof(tc->readbuf.ary) < tr->max_input) {
        size += tr->max_input;
    }
    if ((int)sizeof(tc->writebuf.ary) < tr->max_output) {
        size += tr->max_output;
    }
    return size;
}

rb_econv_t *
rb_econv_alloc(int n_hint)
{
    rb_econv_t *ec;

    if (n_hint <= 0)
        n_hint = 1;

    ec = ALLOC(rb_econv_t);
    ec->flags = 0;
    ec->source_encoding_name = NULL;
    ec->destination_encoding_name = NULL;
    ec->started = 0;
    ec->replacement_str = NULL;
    ec->replacement_len = 0;
    ec->replacement_enc = NULL;
    ec->replacement_allocated = 0;
    ec->num_replacement_converters = 0;
    ec->replacement_converters = NULL;
    ec->in_buf_start = NULL;
    ec->in_data_start = NULL;
    ec->in_data_end = NULL;
    ec->in_buf_end = NULL;
    ec->num_allocated = n_hint;
    ec->num_trans = 0;
    ec->elems = ALLOC_N(rb_econv_elem_t, ec->num_allocated);
    ec->num_finished = 0;
    ec->last_tc = NULL;
    ec->last_error.result = econv_source_buffer_empty;
    ec->last_error.error_tc = NULL;
    ec->last_error.source_encoding = NULL;
    ec->last_error.destination_encoding = NULL;
    ec->last_error.error_bytes_start = NULL;
    ec->last_error.error_bytes_len = 0;
    ec->last_error.readagain_len = 0;
    return ec;
}

void rb_econv_alloc_replacement_converters(rb_econv_t* ec, int num) {
    ec->num_replacement_converters = num;
    ec->replacement_converters = ALLOC_N(rb_econv_replacement_converters, num);
}

void
rb_econv_free(rb_econv_t *ec)
{
    int i;

    if (ec->replacement_allocated) {
        xfree((void *)ec->replacement_str);
        xfree((void *)ec->replacement_enc);
    }
    if (ec->num_replacement_converters) {
      for (i = 0; i < ec->num_replacement_converters; i++) {
        xfree((void *)(ec->replacement_converters[i].destination_encoding_name));
        xfree((void *)(ec->replacement_converters[i].transcoders));
      }
      xfree((void *)ec->replacement_converters);
    }
    for (i = 0; i < ec->num_trans; i++) {
        rb_transcoding_close(ec->elems[i].tc);
        if (ec->elems[i].out_buf_start)
        xfree(ec->elems[i].out_buf_start);
    }
    xfree(ec->in_buf_start);
    xfree(ec->elems);
    xfree(ec);
}

int
rb_econv_add_transcoder_at(rb_econv_t *ec, const rb_transcoder *tr, int i)
{
    int n, j;
    int bufsize = 4096;
    unsigned char *p;

    if (ec->num_trans == ec->num_allocated) {
        n = ec->num_allocated * 2;
        REALLOC_N(ec->elems, rb_econv_elem_t, n);
        ec->num_allocated = n;
    }

    p = xmalloc(bufsize);

    MEMMOVE(ec->elems+i+1, ec->elems+i, rb_econv_elem_t, ec->num_trans-i);

    ec->elems[i].tc = rb_transcoding_open_by_transcoder(tr, 0);
    ec->elems[i].out_buf_start = p;
    ec->elems[i].out_buf_end = p + bufsize;
    ec->elems[i].out_data_start = p;
    ec->elems[i].out_data_end = p;
    ec->elems[i].last_result = econv_source_buffer_empty;

    ec->num_trans++;

    if (!DECORATOR_P(tr->src_encoding, tr->dst_encoding))
        for (j = ec->num_trans-1; i <= j; j--) {
            rb_transcoding *tc = ec->elems[j].tc;
            const rb_transcoder *tr2 = tc->transcoder;
            if (!DECORATOR_P(tr2->src_encoding, tr2->dst_encoding)) {
                ec->last_tc = tc;
                break;
            }
        }

    return 0;
}
static int
trans_sweep(rb_econv_t *ec,
    const unsigned char **input_ptr, const unsigned char *input_stop,
    unsigned char **output_ptr, unsigned char *output_stop,
    int flags,
    int start)
{
    int try;
    int i, f;

    const unsigned char **ipp, *is, *iold;
    unsigned char **opp, *os, *oold;
    rb_econv_result_t res;

    try = 1;
    while (try) {
        try = 0;
        for (i = start; i < ec->num_trans; i++) {
            rb_econv_elem_t *te = &ec->elems[i];

            if (i == 0) {
                ipp = input_ptr;
                is = input_stop;
            }
            else {
                rb_econv_elem_t *prev_te = &ec->elems[i-1];
                ipp = (const unsigned char **)&prev_te->out_data_start;
                is = prev_te->out_data_end;
            }

            if (i == ec->num_trans-1) {
                opp = output_ptr;
                os = output_stop;
            }
            else {
                if (te->out_buf_start != te->out_data_start) {
                    ssize_t len = te->out_data_end - te->out_data_start;
                    ssize_t off = te->out_data_start - te->out_buf_start;
                    MEMMOVE(te->out_buf_start, te->out_data_start, unsigned char, len);
                    te->out_data_start = te->out_buf_start;
                    te->out_data_end -= off;
                }
                opp = &te->out_data_end;
                os = te->out_buf_end;
            }

            f = flags;
            if (ec->num_finished != i)
                f |= ECONV_PARTIAL_INPUT;
            if (i == 0 && (flags & ECONV_AFTER_OUTPUT)) {
                start = 1;
                flags &= ~ECONV_AFTER_OUTPUT;
            }
            if (i != 0)
                f &= ~ECONV_AFTER_OUTPUT;
            iold = *ipp;
            oold = *opp;
            te->last_result = res = rb_transcoding_convert(te->tc, ipp, is, opp, os, f);
            if (iold != *ipp || oold != *opp)
                try = 1;

            switch (res) {
              case econv_invalid_byte_sequence:
              case econv_incomplete_input:
              case econv_undefined_conversion:
              case econv_after_output:
                return i;

              case econv_destination_buffer_full:
              case econv_source_buffer_empty:
                break;

              case econv_finished:
                ec->num_finished = i+1;
                break;
            }
        }
    }
    return -1;
}

static rb_econv_result_t
rb_trans_conv(rb_econv_t *ec,
    const unsigned char **input_ptr, const unsigned char *input_stop,
    unsigned char **output_ptr, unsigned char *output_stop,
    int flags,
    int *result_position_ptr)
{
    int i;
    int needreport_index;
    int sweep_start;

    unsigned char empty_buf;
    unsigned char *empty_ptr = &empty_buf;

    if (!input_ptr) {
        input_ptr = (const unsigned char **)&empty_ptr;
        input_stop = empty_ptr;
    }

    if (!output_ptr) {
        output_ptr = &empty_ptr;
        output_stop = empty_ptr;
    }

    if (ec->elems[0].last_result == econv_after_output)
        ec->elems[0].last_result = econv_source_buffer_empty;

    needreport_index = -1;
    for (i = ec->num_trans-1; 0 <= i; i--) {
        switch (ec->elems[i].last_result) {
          case econv_invalid_byte_sequence:
          case econv_incomplete_input:
          case econv_undefined_conversion:
          case econv_after_output:
          case econv_finished:
            sweep_start = i+1;
            needreport_index = i;
            goto found_needreport;

          case econv_destination_buffer_full:
          case econv_source_buffer_empty:
            break;

          default:
            // TODO: remove
            // rb_bug("unexpected transcode last result");
            return 0;
        }
    }

    /* /^[sd]+$/ is confirmed.  but actually /^s*d*$/. */

    if (ec->elems[ec->num_trans-1].last_result == econv_destination_buffer_full &&
        (flags & ECONV_AFTER_OUTPUT)) {
        rb_econv_result_t res;

        res = rb_trans_conv(ec, NULL, NULL, output_ptr, output_stop,
                (flags & ~ECONV_AFTER_OUTPUT)|ECONV_PARTIAL_INPUT,
                result_position_ptr);

        if (res == econv_source_buffer_empty)
            return econv_after_output;
        return res;
    }

    sweep_start = 0;

  found_needreport:

    do {
        needreport_index = trans_sweep(ec, input_ptr, input_stop, output_ptr, output_stop, flags, sweep_start);
        sweep_start = needreport_index + 1;
    } while (needreport_index != -1 && needreport_index != ec->num_trans-1);

    for (i = ec->num_trans-1; 0 <= i; i--) {
        if (ec->elems[i].last_result != econv_source_buffer_empty) {
            rb_econv_result_t res = ec->elems[i].last_result;
            if (res == econv_invalid_byte_sequence ||
                res == econv_incomplete_input ||
                res == econv_undefined_conversion ||
                res == econv_after_output) {
                ec->elems[i].last_result = econv_source_buffer_empty;
            }
            if (result_position_ptr)
                *result_position_ptr = i;
            return res;
        }
    }
    if (result_position_ptr)
        *result_position_ptr = -1;
    return econv_source_buffer_empty;
}

static rb_econv_result_t
econv_convert0(rb_econv_t *ec,
    const unsigned char **input_ptr, const unsigned char *input_stop,
    unsigned char **output_ptr, unsigned char *output_stop,
    int flags)
{
    rb_econv_result_t res;
    int result_position;
    int has_output = 0;

    memset(&ec->last_error, 0, sizeof(ec->last_error));

    if (ec->num_trans == 0) {
        size_t len;
        if (ec->in_buf_start && ec->in_data_start != ec->in_data_end) {
            if (output_stop - *output_ptr < ec->in_data_end - ec->in_data_start) {
                len = output_stop - *output_ptr;
                memcpy(*output_ptr, ec->in_data_start, len);
                *output_ptr = output_stop;
                ec->in_data_start += len;
                res = econv_destination_buffer_full;
                goto gotresult;
            }
            len = ec->in_data_end - ec->in_data_start;
            memcpy(*output_ptr, ec->in_data_start, len);
            *output_ptr += len;
            ec->in_data_start = ec->in_data_end = ec->in_buf_start;
            if (flags & ECONV_AFTER_OUTPUT) {
                res = econv_after_output;
                goto gotresult;
            }
        }
        if (output_stop - *output_ptr < input_stop - *input_ptr) {
            len = output_stop - *output_ptr;
        }
        else {
            len = input_stop - *input_ptr;
        }
        if (0 < len && (flags & ECONV_AFTER_OUTPUT)) {
            *(*output_ptr)++ = *(*input_ptr)++;
            res = econv_after_output;
            goto gotresult;
        }
        memcpy(*output_ptr, *input_ptr, len);
        *output_ptr += len;
        *input_ptr += len;
        if (*input_ptr != input_stop)
            res = econv_destination_buffer_full;
        else if (flags & ECONV_PARTIAL_INPUT)
            res = econv_source_buffer_empty;
        else
            res = econv_finished;
        goto gotresult;
    }

    if (ec->elems[ec->num_trans-1].out_data_start) {
        unsigned char *data_start = ec->elems[ec->num_trans-1].out_data_start;
        unsigned char *data_end = ec->elems[ec->num_trans-1].out_data_end;
        if (data_start != data_end) {
            size_t len;
            if (output_stop - *output_ptr < data_end - data_start) {
                len = output_stop - *output_ptr;
                memcpy(*output_ptr, data_start, len);
                *output_ptr = output_stop;
                ec->elems[ec->num_trans-1].out_data_start += len;
                res = econv_destination_buffer_full;
                goto gotresult;
            }
            len = data_end - data_start;
            memcpy(*output_ptr, data_start, len);
            *output_ptr += len;
            ec->elems[ec->num_trans-1].out_data_start =
                ec->elems[ec->num_trans-1].out_data_end =
                ec->elems[ec->num_trans-1].out_buf_start;
            has_output = 1;
        }
    }

    if (ec->in_buf_start &&
        ec->in_data_start != ec->in_data_end) {
        res = rb_trans_conv(ec, (const unsigned char **)&ec->in_data_start, ec->in_data_end, output_ptr, output_stop,
                (flags&~ECONV_AFTER_OUTPUT)|ECONV_PARTIAL_INPUT, &result_position);
        if (res != econv_source_buffer_empty)
            goto gotresult;
    }

    if (has_output &&
        (flags & ECONV_AFTER_OUTPUT) &&
        *input_ptr != input_stop) {
        input_stop = *input_ptr;
        res = rb_trans_conv(ec, input_ptr, input_stop, output_ptr, output_stop, flags, &result_position);
        if (res == econv_source_buffer_empty)
            res = econv_after_output;
    }
    else if ((flags & ECONV_AFTER_OUTPUT) ||
        ec->num_trans == 1) {
        res = rb_trans_conv(ec, input_ptr, input_stop, output_ptr, output_stop, flags, &result_position);
    }
    else {
        flags |= ECONV_AFTER_OUTPUT;
        do {
            res = rb_trans_conv(ec, input_ptr, input_stop, output_ptr, output_stop, flags, &result_position);
        } while (res == econv_after_output);
    }

  gotresult:
    ec->last_error.result = res;
    if (res == econv_invalid_byte_sequence ||
        res == econv_incomplete_input ||
        res == econv_undefined_conversion) {
        rb_transcoding *error_tc = ec->elems[result_position].tc;
        ec->last_error.error_tc = error_tc;
        ec->last_error.source_encoding = error_tc->transcoder->src_encoding;
        ec->last_error.destination_encoding = error_tc->transcoder->dst_encoding;
        ec->last_error.error_bytes_start = TRANSCODING_READBUF(error_tc);
        ec->last_error.error_bytes_len = error_tc->recognized_len;
        ec->last_error.readagain_len = error_tc->readagain_len;
    }

    return res;
}

rb_econv_t* rb_econv_replacement_converter_open(rb_econv_t* main_ec,
    const char* sname, const char* dname)
{
    rb_econv_t* ec;
    int i, j;

    for(i = 0; i < main_ec->num_replacement_converters; i++) {
      rb_econv_replacement_converters* repl_converter;
      repl_converter = main_ec->replacement_converters + i;

      if(!encoding_equal(dname, repl_converter->destination_encoding_name))
        continue;

      int num = repl_converter->num_transcoders;
      ec = rb_econv_alloc(num);

      for(j = 0; j < num; j++) {
        rb_transcoder* tr = repl_converter->transcoders[j];

        if(rb_econv_add_transcoder_at(ec, tr, j) == -1) {
          rb_econv_free(ec);
          return NULL;
        }
      }

      ec->flags = 0;
      ec->source_encoding_name = sname;
      ec->destination_encoding_name = dname;

      return ec;
    }

    return NULL;
}

static unsigned char *
allocate_converted_string(rb_econv_t* main_ec, const char *sname, const char *dname,
        const unsigned char *str, size_t len,
        unsigned char *caller_dst_buf, size_t caller_dst_bufsize,
        size_t *dst_len_ptr)
{
    unsigned char *dst_str;
    size_t dst_len;
    size_t dst_bufsize;

    rb_econv_t *ec;
    rb_econv_result_t res;

    const unsigned char *sp;
    unsigned char *dp;

    if (caller_dst_buf)
        dst_bufsize = caller_dst_bufsize;
    else if (len == 0)
        dst_bufsize = 1;
    else
        dst_bufsize = len;

    ec = rb_econv_replacement_converter_open(main_ec, sname, dname);
    if (ec == NULL)
        return NULL;
    if (caller_dst_buf)
        dst_str = caller_dst_buf;
    else
        dst_str = xmalloc(dst_bufsize);
    dst_len = 0;
    sp = str;
    dp = dst_str+dst_len;
    res = econv_convert(ec, &sp, str+len, &dp, dst_str+dst_bufsize, 0);
    dst_len = dp - dst_str;
    while (res == econv_destination_buffer_full) {
        if (SIZE_MAX/2 < dst_bufsize) {
            goto fail;
        }
        dst_bufsize *= 2;
        if (dst_str == caller_dst_buf) {
            unsigned char *tmp;
            tmp = xmalloc(dst_bufsize);
            memcpy(tmp, dst_str, dst_bufsize/2);
            dst_str = tmp;
        }
        else {
            dst_str = xrealloc(dst_str, dst_bufsize);
        }
        dp = dst_str+dst_len;
        res = econv_convert(ec, &sp, str+len, &dp, dst_str+dst_bufsize, 0);
        dst_len = dp - dst_str;
    }
    if (res != econv_finished) {
        goto fail;
    }
    rb_econv_free(ec);
    *dst_len_ptr = dst_len;
    return dst_str;

  fail:
    if (dst_str != caller_dst_buf)
        xfree(dst_str);
    rb_econv_free(ec);
    return NULL;
}

const char *
rb_econv_encoding_to_insert_output(rb_econv_t *ec)
{
    rb_transcoding *tc = ec->last_tc;
    const rb_transcoder *tr;

    if (tc == NULL)
        return "";

    tr = tc->transcoder;

    if (tr->asciicompat_type == asciicompat_encoder)
        return tr->src_encoding;
    return tr->dst_encoding;
}

/* result: 0:success -1:failure */
int
rb_econv_insert_output(rb_econv_t *ec,
    const unsigned char *str, size_t len, const char *str_encoding)
{
    const char *insert_encoding = rb_econv_encoding_to_insert_output(ec);
    unsigned char insert_buf[4096];
    const unsigned char *insert_str = NULL;
    size_t insert_len;

    int last_trans_index;
    rb_transcoding *tc;

    unsigned char **buf_start_p;
    unsigned char **data_start_p;
    unsigned char **data_end_p;
    unsigned char **buf_end_p;

    size_t need;

    ec->started = 1;

    if (len == 0)
        return 0;

    if (encoding_equal(insert_encoding, str_encoding)) {
        insert_str = str;
        insert_len = len;
    }
    else {
        insert_str = allocate_converted_string(ec, str_encoding, insert_encoding,
                str, len, insert_buf, sizeof(insert_buf), &insert_len);
        if (insert_str == NULL)
            return -1;
    }

    need = insert_len;

    last_trans_index = ec->num_trans-1;
    if (ec->num_trans == 0) {
        tc = NULL;
        buf_start_p = &ec->in_buf_start;
        data_start_p = &ec->in_data_start;
        data_end_p = &ec->in_data_end;
        buf_end_p = &ec->in_buf_end;
    }
    else if (ec->elems[last_trans_index].tc->transcoder->asciicompat_type == asciicompat_encoder) {
        tc = ec->elems[last_trans_index].tc;
        need += tc->readagain_len;
        if (need < insert_len)
            goto fail;
        if (last_trans_index == 0) {
            buf_start_p = &ec->in_buf_start;
            data_start_p = &ec->in_data_start;
            data_end_p = &ec->in_data_end;
            buf_end_p = &ec->in_buf_end;
        }
        else {
            rb_econv_elem_t *ee = &ec->elems[last_trans_index-1];
            buf_start_p = &ee->out_buf_start;
            data_start_p = &ee->out_data_start;
            data_end_p = &ee->out_data_end;
            buf_end_p = &ee->out_buf_end;
        }
    }
    else {
        rb_econv_elem_t *ee = &ec->elems[last_trans_index];
        buf_start_p = &ee->out_buf_start;
        data_start_p = &ee->out_data_start;
        data_end_p = &ee->out_data_end;
        buf_end_p = &ee->out_buf_end;
        tc = ec->elems[last_trans_index].tc;
    }

    if (*buf_start_p == NULL) {
        unsigned char *buf = xmalloc(need);
        *buf_start_p = buf;
        *data_start_p = buf;
        *data_end_p = buf;
        *buf_end_p = buf+need;
    }
    else if ((size_t)(*buf_end_p - *data_end_p) < need) {
        MEMMOVE(*buf_start_p, *data_start_p, unsigned char, *data_end_p - *data_start_p);
        *data_end_p = *buf_start_p + (*data_end_p - *data_start_p);
        *data_start_p = *buf_start_p;
        if ((size_t)(*buf_end_p - *data_end_p) < need) {
            unsigned char *buf;
            size_t s = (*data_end_p - *buf_start_p) + need;
            if (s < need)
                goto fail;
            buf = xrealloc(*buf_start_p, s);
            *data_start_p = buf;
            *data_end_p = buf + (*data_end_p - *buf_start_p);
            *buf_start_p = buf;
            *buf_end_p = buf + s;
        }
    }

    memcpy(*data_end_p, insert_str, insert_len);
    *data_end_p += insert_len;
    if (tc && tc->transcoder->asciicompat_type == asciicompat_encoder) {
        memcpy(*data_end_p, TRANSCODING_READBUF(tc)+tc->recognized_len, tc->readagain_len);
        *data_end_p += tc->readagain_len;
        tc->readagain_len = 0;
    }

    if (insert_str != str && insert_str != insert_buf)
        xfree((void*)insert_str);
    return 0;

  fail:
    if (insert_str != str && insert_str != insert_buf)
        xfree((void*)insert_str);
    return -1;
}

static int
output_replacement_character(rb_econv_t *ec)
{
    int ret;

    ret = rb_econv_insert_output(ec, ec->replacement_str, ec->replacement_len, ec->replacement_enc);
    if (ret == -1)
        return -1;

    return 0;
}

static int
output_hex_charref(rb_econv_t *ec)
{
    int ret;
    unsigned char utfbuf[1024];
    const unsigned char *utf;
    size_t utf_len;
    int utf_allocated = 0;
    char charef_buf[16];
    const unsigned char *p;

    if (encoding_equal(ec->last_error.source_encoding, "UTF-32BE")) {
        utf = ec->last_error.error_bytes_start;
        utf_len = ec->last_error.error_bytes_len;
    }
    else {
        utf = allocate_converted_string(ec, ec->last_error.source_encoding, "UTF-32BE",
                ec->last_error.error_bytes_start, ec->last_error.error_bytes_len,
                utfbuf, sizeof(utfbuf),
                &utf_len);
        if (!utf)
            return -1;
        if (utf != utfbuf && utf != ec->last_error.error_bytes_start)
            utf_allocated = 1;
      return -1;
    }

    if (utf_len % 4 != 0)
        goto fail;

    p = utf;
    while (4 <= utf_len) {
        unsigned int u = 0;
        u += p[0] << 24;
        u += p[1] << 16;
        u += p[2] << 8;
        u += p[3];
        snprintf(charef_buf, sizeof(charef_buf), "&#x%X;", u);

        ret = rb_econv_insert_output(ec, (unsigned char *)charef_buf, strlen(charef_buf), "US-ASCII");
        if (ret == -1)
            goto fail;

        p += 4;
        utf_len -= 4;
    }

    if (utf_allocated)
        xfree((void *)utf);
    return 0;

  fail:
    if (utf_allocated)
        xfree((void *)utf);
    return -1;
}

rb_econv_result_t
econv_convert(rb_econv_t *ec,
    const unsigned char **input_ptr, const unsigned char *input_stop,
    unsigned char **output_ptr, unsigned char *output_stop,
    int flags)
{
    rb_econv_result_t ret;

    unsigned char empty_buf;
    unsigned char *empty_ptr = &empty_buf;

    ec->started = 1;

    if (!input_ptr) {
        input_ptr = (const unsigned char **)&empty_ptr;
        input_stop = empty_ptr;
    }

    if (!output_ptr) {
        output_ptr = &empty_ptr;
        output_stop = empty_ptr;
    }

  resume:
    ret = econv_convert0(ec, input_ptr, input_stop, output_ptr, output_stop, flags);

    if (ret == econv_invalid_byte_sequence ||
        ret == econv_incomplete_input) {
        /* deal with invalid byte sequence */
        /* todo: add more alternative behaviors */
        switch (ec->flags & ECONV_INVALID_MASK) {
          case ECONV_INVALID_REPLACE:
            if (output_replacement_character(ec) == 0)
                goto resume;
        }
    }

    if (ret == econv_undefined_conversion) {
        /* valid character in source encoding
         * but no related character(s) in destination encoding */
        /* todo: add more alternative behaviors */
        switch (ec->flags & ECONV_UNDEF_MASK) {
          case ECONV_UNDEF_REPLACE:
            if (output_replacement_character(ec) == 0)
                goto resume;
            break;

          case ECONV_UNDEF_HEX_CHARREF:
            if (output_hex_charref(ec) == 0)
                goto resume;
            break;
        }
    }

    return ret;
}

size_t
rb_econv_memsize(rb_econv_t *ec)
{
    size_t size = sizeof(rb_econv_t);
    int i;

    if (ec->replacement_allocated) {
        size += ec->replacement_len;
    }
    for (i = 0; i < ec->num_trans; i++) {
        size += rb_transcoding_memsize(ec->elems[i].tc);

        if (ec->elems[i].out_buf_start) {
            size += ec->elems[i].out_buf_end - ec->elems[i].out_buf_start;
        }
    }
    size += ec->in_buf_end - ec->in_buf_start;
    size += sizeof(rb_econv_elem_t) * ec->num_allocated;

    return size;
}

int
rb_econv_putbackable(rb_econv_t *ec)
{
    if (ec->num_trans == 0)
        return 0;
#if SIZEOF_SIZE_T > SIZEOF_INT
    if (ec->elems[0].tc->readagain_len > INT_MAX) return INT_MAX;
#endif
    return (int)ec->elems[0].tc->readagain_len;
}

void
rb_econv_putback(rb_econv_t *ec, unsigned char *p, int n)
{
    rb_transcoding *tc;
    if (ec->num_trans == 0 || n == 0)
        return;
    tc = ec->elems[0].tc;
    memcpy(p, TRANSCODING_READBUF(tc) + tc->recognized_len + tc->readagain_len - n, n);
    tc->readagain_len -= n;
}

static void
more_output_buffer(
        VALUE destination,
        unsigned char *(*resize_destination)(VALUE, size_t, size_t),
        int max_output,
        unsigned char **out_start_ptr,
        unsigned char **out_pos,
        unsigned char **out_stop_ptr)
{
    size_t len = (*out_pos - *out_start_ptr);
    size_t new_len = (len + max_output) * 2;
    *out_start_ptr = resize_destination(destination, len, new_len);
    *out_pos = *out_start_ptr + len;
    *out_stop_ptr = *out_start_ptr + new_len;
}
