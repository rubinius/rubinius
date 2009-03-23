/* -*- C -*-
 * $Id: defs.h 15780 2008-03-14 08:04:45Z nobu $
 */

#ifndef DEFS_H
#define DEFS_H

#include "ruby.h"
#include <sys/types.h>

#if defined(HAVE_SYS_CDEFS_H)
# include <sys/cdefs.h>
#endif
#if !defined(__BEGIN_DECLS)
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#endif /* DEFS_H */
