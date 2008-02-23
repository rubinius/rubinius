/* -*- C -*-
 * $Id: defs.h 11708 2007-02-12 23:01:19Z shyouhei $
 */

#ifndef DIGEST_DEFS_H
#define DIGEST_DEFS_H

#include "ruby.h"
#include <sys/types.h>

#include <sys/cdefs.h>
/*
 * #if defined(HAVE_SYS_CDEFS_H)
 * # include <sys/cdefs.h>
 * #endif
 * #if !defined(__BEGIN_DECLS)
 * # define __BEGIN_DECLS
 * # define __END_DECLS
 * #endif
 */

#include <inttypes.h>
/*
 * #if defined(HAVE_INTTYPES_H)
 * # include <inttypes.h>
 * #elif !defined __CYGWIN__ || !defined __uint8_t_defined
 *   typedef unsigned char uint8_t;
 *   typedef unsigned int  uint32_t;
 * # if SIZEOF_LONG == 8
 *   typedef unsigned long uint64_t;
 * # elif SIZEOF_LONG_LONG == 8
 *   typedef unsigned LONG_LONG uint64_t;
 * # else
 * #  define NO_UINT64_T
 * # endif
 * #endif
 */

/* Define if processor uses big-endian word */
/*
#ifdef __BIG_ENDIAN__
#define WORDS_BIGENDIAN 
#else */ /* !__BIG_ENDIAN__ */
/* #undef WORDS_BIGENDIAN */
/* #endif */ /* __BIG_ENDIAN__ */

#endif /* DIGEST_DEFS_H */
