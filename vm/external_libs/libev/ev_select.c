/*
 * libev select fd activity backend
 *
 * Copyright (c) 2007 Marc Alexander Lehmann <libev@schmorp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 * 
 *   1.  Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 * 
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MER-
 * CHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTH-
 * ERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU General Public License ("GPL") version 2 or any later version,
 * in which case the provisions of the GPL are applicable instead of
 * the above. If you wish to allow the use of your version of this file
 * only under the terms of the GPL and not to allow others to use your
 * version of this file under the BSD license, indicate your decision
 * by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL. If you do not delete the
 * provisions above, a recipient may use your version of this file under
 * either the BSD or the GPL.
 */

#ifndef _WIN32
/* for unix systems */
# include <sys/select.h>
# include <inttypes.h>
#endif

#ifndef EV_SELECT_USE_FD_SET
# ifdef NFDBITS
#  define EV_SELECT_USE_FD_SET 0
# else
#  define EV_SELECT_USE_FD_SET 1
# endif
#endif

#if EV_SELECT_IS_WINSOCKET
# undef EV_SELECT_USE_FD_SET
# define EV_SELECT_USE_FD_SET 1
# undef EINTR
# define EINTR WSAEINTR
# undef EBADF
# define EBADF WSAENOTSOCK
# undef ENOMEM
# define ENOMEM (errno + 1)
#endif

#if !EV_SELECT_USE_FD_SET
# define NFDBYTES (NFDBITS / 8)
#endif

#include <string.h>

static void
select_modify (EV_P_ int fd, int oev, int nev)
{
  if (oev == nev)
    return;

  {
#if EV_SELECT_USE_FD_SET

    #if EV_SELECT_IS_WINSOCKET
    SOCKET handle = anfds [fd].handle;
    #else
    int handle = fd;
    #endif

    if (nev & EV_READ)
      FD_SET (handle, (fd_set *)vec_ri);
    else
      FD_CLR (handle, (fd_set *)vec_ri);

    if (nev & EV_WRITE)
      FD_SET (handle, (fd_set *)vec_wi);
    else
      FD_CLR (handle, (fd_set *)vec_wi);

#else

    int word = fd / NFDBITS;
    int mask = 1UL << (fd % NFDBITS);

    if (expect_false (vec_max < word + 1))
      {
        int new_max = word + 1;

        vec_ri = ev_realloc (vec_ri, new_max * NFDBYTES);
        vec_ro = ev_realloc (vec_ro, new_max * NFDBYTES); /* could free/malloc */
        vec_wi = ev_realloc (vec_wi, new_max * NFDBYTES);
        vec_wo = ev_realloc (vec_wo, new_max * NFDBYTES); /* could free/malloc */

        for (; vec_max < new_max; ++vec_max)
          ((fd_mask *)vec_ri)[vec_max] =
          ((fd_mask *)vec_wi)[vec_max] = 0;
      }

    ((fd_mask *)vec_ri) [word] |= mask;
    if (!(nev & EV_READ))
      ((fd_mask *)vec_ri) [word] &= ~mask;

    ((fd_mask *)vec_wi) [word] |= mask;
    if (!(nev & EV_WRITE))
      ((fd_mask *)vec_wi) [word] &= ~mask;
#endif
  }
}

static void
select_poll (EV_P_ ev_tstamp timeout)
{
  struct timeval tv;
  int res;

#if EV_SELECT_USE_FD_SET
  memcpy (vec_ro, vec_ri, sizeof (fd_set));
  memcpy (vec_wo, vec_wi, sizeof (fd_set));
#else
  memcpy (vec_ro, vec_ri, vec_max * NFDBYTES);
  memcpy (vec_wo, vec_wi, vec_max * NFDBYTES);
#endif

  tv.tv_sec  = (long)timeout;
  tv.tv_usec = (long)((timeout - (ev_tstamp)tv.tv_sec) * 1e6);

  res = select (vec_max * NFDBITS, (fd_set *)vec_ro, (fd_set *)vec_wo, 0, &tv);

  if (expect_false (res < 0))
    {
      #if EV_SELECT_IS_WINSOCKET
      errno = WSAGetLastError ();
      #endif

      if (errno == EBADF)
        fd_ebadf (EV_A);
      else if (errno == ENOMEM && !syserr_cb)
        fd_enomem (EV_A);
      else if (errno != EINTR)
        syserr ("(libev) select");

      return;
    }

#if EV_SELECT_USE_FD_SET

  {
    int fd;

    for (fd = 0; fd < anfdmax; ++fd)
      if (anfds [fd].events)
        {
          int events = 0;
          #if EV_SELECT_IS_WINSOCKET
          SOCKET handle = anfds [fd].handle;
          #else
          int handle = fd;
          #endif

          if (FD_ISSET (handle, (fd_set *)vec_ro)) events |= EV_READ;
          if (FD_ISSET (handle, (fd_set *)vec_wo)) events |= EV_WRITE;

          if (expect_true (events))
            fd_event (EV_A_ fd, events);
        }
  }

#else

  {
    int word, bit;
    for (word = vec_max; word--; )
      {
        fd_mask word_r = ((fd_mask *)vec_ro) [word];
        fd_mask word_w = ((fd_mask *)vec_wo) [word];

        if (word_r || word_w)
          for (bit = NFDBITS; bit--; )
            {
              fd_mask mask = 1UL << bit;
              int events = 0;

              events |= word_r & mask ? EV_READ  : 0;
              events |= word_w & mask ? EV_WRITE : 0;

              if (expect_true (events))
                fd_event (EV_A_ word * NFDBITS + bit, events);
            }
      }
  }

#endif
}

int inline_size
select_init (EV_P_ int flags)
{
  backend_fudge  = 0.; /* posix says this is zero */
  backend_modify = select_modify;
  backend_poll   = select_poll;

#if EV_SELECT_USE_FD_SET
  vec_max = FD_SETSIZE / 32;
  vec_ri  = ev_malloc (sizeof (fd_set)); FD_ZERO ((fd_set *)vec_ri);
  vec_ro  = ev_malloc (sizeof (fd_set));
  vec_wi  = ev_malloc (sizeof (fd_set)); FD_ZERO ((fd_set *)vec_wi);
  vec_wo  = ev_malloc (sizeof (fd_set));
#else
  vec_max = 0;
  vec_ri  = 0; 
  vec_ri  = 0;   
  vec_wo  = 0; 
  vec_wo  = 0; 
#endif

  return EVBACKEND_SELECT;
}

void inline_size
select_destroy (EV_P)
{
  ev_free (vec_ri);
  ev_free (vec_ro);
  ev_free (vec_wi);
  ev_free (vec_wo);
}


