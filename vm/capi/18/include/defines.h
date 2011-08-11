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

#define HAVE_TYPE_RB_BLOCKING_FUNCTION_T   1
#define HAVE_RB_THREAD_BLOCKING_REGION     1

#define HAVE_RB_IO_T                       1
#define HAVE_RB_IO_FD                      1

#define HAVE_RB_ERRINFO                    1

#define HAVE_RB_STR_PTR_READONLY           1
