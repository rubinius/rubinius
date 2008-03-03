#define EH_FRAME_FLAGS "@EH_FRAME_FLAGS@"

#cmakedefine FFI_DEBUG 1
#cmakedefine FFI_NO_RAW_API 1
#cmakedefine FFI_NO_STRUCTS 1

#cmakedefine HAVE_ALLOCA_H 1
#cmakedefine HAVE_AS_CFI_PSEUDO_OP 1
#cmakedefine HAVE_AS_REGISTER_PSEUDO_OP 1
#cmakedefine HAVE_AS_SPARC_UA_PCREL 1
#cmakedefine HAVE_HIDDEN_VISIBILITY_ATTRIBUTE 1
#cmakedefine HAVE_LONG_DOUBLE 1
#cmakedefine HAVE_MEMCPY 1
#cmakedefine HAVE_MEMORY_H 1
#cmakedefine HAVE_MMAP 1
#cmakedefine HAVE_MMAP_ANON 1
#cmakedefine HAVE_MMAP_DEV_ZERO 1
#cmakedefine HAVE_MMAP_FILE 1
#cmakedefine HAVE_RO_EH_FRAME 1
#cmakedefine HAVE_SYS_MMAN_H 1

#define STDC_HEADERS 1
#cmakedefine USING_PURIFY 1

#define WORDS_BIGENDIAN @WORDS_BIGENDIAN@

#ifdef HAVE_HIDDEN_VISIBILITY_ATTRIBUTE
#ifdef LIBFFI_ASM
#define FFI_HIDDEN(name) .hidden name
#else
#define FFI_HIDDEN __attribute__ ((visibility ("hidden")))
#endif
#else
#ifdef LIBFFI_ASM
#define FFI_HIDDEN(name)
#else
#define FFI_HIDDEN
#endif
#endif
