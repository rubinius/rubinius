#ifndef RUBYSPEC_CAPI_MRI_H
#define RUBYSPEC_CAPI_MRI_H

// #undef any HAVE_ defines that MRI does not have.
#undef HAVE_RB_HASH_LOOKUP
#undef HAVE_RB_HASH_SIZE

#undef HAVE_RB_OBJ_FROZEN_P

#undef HAVE_RB_STR_PTR
#undef HAVE_RB_STR_PTR_READONLY

#undef HAVE_THREAD_BLOCKING_REGION

/* undefs for 1.8.6 */
#if RUBY_VERSION_MAJOR == 1 && RUBY_VERSION_MINOR == 8 && RUBY_VERSION_TEENY <= 6
#undef HAVE_RB_STR_SET_LEN
#endif

#endif
