#ifndef RBX_CAPI_RUBY_DEFINES_H
#define RBX_CAPI_RUBY_DEFINES_H

/* Stub file provided for C extensions that expect it. All regular
 * defines and prototypes are in ruby.h
 */

#define RUBY                               1
#define RUBINIUS                           1

#define HAVE_STDARG_PROTOTYPES             1

/* These are defines directly related to MRI C-API symbols that the
 * mkmf.rb discovery code (e.g. have_func("rb_str_set_len")) would
 * attempt to find by linking against libruby. Rubinius does not
 * have an appropriate lib to link against, so we are adding these
 * explicit defines for now.
 */
#define HAVE_RB_STR_SET_LEN                1
#define HAVE_RB_DEFINE_ALLOC_FUNC          1

#define HAVE_RB_HASH_FOREACH               1
#define HAVE_RB_HASH_ASET                  1

#define HAVE_RUBY_ENCODING_H               1

#define HAVE_RB_ENCDB_ALIAS                1

#endif
