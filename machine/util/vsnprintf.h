/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * IMPORTANT NOTE:
 * --------------
 * From ftp://ftp.cs.berkeley.edu/pub/4bsd/README.Impt.License.Change
 * paragraph 3 above is now null and void.
 */

/* SNPRINTF.C
 * fjc 7-31-97 Modified by Mib Software to be a standalone snprintf.c module.
 *      http://www.mibsoftware.com
 * Mib Software does not warrant this software any differently than the
 * University of California, Berkeley as described above.  All warranties
 * are disclaimed.  Use this software at your own risk.
 *
 *      All code referencing FILE * functions was eliminated, since it could
 *      never be called.  All header files and necessary files are collapsed
 *      into one file, internal functions are declared static.  This should
 *      allow inclusion into libraries with less chance of namespace collisions.
 *
 *      snprintf should be the only externally visible item.
 *
 *      As of 7-31-97 FLOATING_POINT is NOT provided.  The code is somewhat
 *        non-portable, so it is disabled.
 */
#ifndef VSNPRINTF_H
#define VSNPRINTF_H

#if defined(__cplusplus)
extern "C" {
#endif

int ruby_vsnprintf(char *str, size_t n, const char *fmt, va_list ap);
int ruby_snprintf(char *str, size_t n, char const *fmt, ...);

#if defined(__cplusplus)
}
#endif

#endif
