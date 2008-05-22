/*
 * libev epoll fd activity backend
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

/*
 * general notes about epoll:
 *
 * a) epoll silently removes fds from the fd set. as nothing tells us
 *    that an fd has been removed otherwise, we have to continually
 *    "rearm" fds that we suspect *might* have changed (same
 *    problem with kqueue, but much less costly there).
 * b) the fact that ADD != MOD creates a lot of extra syscalls due to a)
 *    and seems not to have any advantage.
 * c) the inability to handle fork or file descriptors (think dup)
 *    limits the applicability over poll, so this is not a generic
 *    poll replacement.
 *
 * lots of "weird code" and complication handling in this file is due
 * to these design problems with epoll, as we try very hard to avoid
 * epoll_ctl syscalls for common usage patterns.
 */

#include <sys/epoll.h>

static void
epoll_modify (EV_P_ int fd, int oev, int nev)
{
  struct epoll_event ev;

  /*
   * we handle EPOLL_CTL_DEL by ignoring it here
   * on the assumption that the fd is gone anyways
   * if that is wrong, we have to handle the spurious
   * event in epoll_poll.
   */
  if (!nev)
    return;

  ev.data.u64 = fd; /* use u64 to fully initialise the struct, for nicer strace etc. */
  ev.events   = (nev & EV_READ  ? EPOLLIN  : 0)
              | (nev & EV_WRITE ? EPOLLOUT : 0);

  if (expect_true (!epoll_ctl (backend_fd, oev ? EPOLL_CTL_MOD : EPOLL_CTL_ADD, fd, &ev)))
    return;

  if (expect_true (errno == ENOENT))
    {
      /* on ENOENT the fd went away, so try to do the right thing */
      if (!nev)
        return;

      if (!epoll_ctl (backend_fd, EPOLL_CTL_ADD, fd, &ev))
        return;
    }
  else if (expect_true (errno == EEXIST))
    {
      /* on EEXIST we ignored a previous DEL */
      if (!epoll_ctl (backend_fd, EPOLL_CTL_MOD, fd, &ev))
        return;
    }

  fd_kill (EV_A_ fd);
}

static void
epoll_poll (EV_P_ ev_tstamp timeout)
{
  int i;
  int eventcnt = epoll_wait (backend_fd, epoll_events, epoll_eventmax, (int)ceil (timeout * 1000.));

  if (expect_false (eventcnt < 0))
    {
      if (errno != EINTR)
        syserr ("(libev) epoll_wait");

      return;
    }

  for (i = 0; i < eventcnt; ++i)
    {
      struct epoll_event *ev = epoll_events + i;

      int fd   = ev->data.u64;
      int got  = (ev->events & (EPOLLOUT | EPOLLERR | EPOLLHUP) ? EV_WRITE : 0)
               | (ev->events & (EPOLLIN  | EPOLLERR | EPOLLHUP) ? EV_READ  : 0);
      int want = anfds [fd].events;

      if (expect_false (got & ~want))
        {
          /* we received an event but are not interested in it, try mod or del */
          ev->events = (want & EV_READ  ? EPOLLIN  : 0)
                     | (want & EV_WRITE ? EPOLLOUT : 0);

          epoll_ctl (backend_fd, want ? EPOLL_CTL_MOD : EPOLL_CTL_DEL, fd, ev);
        }

      fd_event (EV_A_ fd, got);
    }

  /* if the receive array was full, increase its size */
  if (expect_false (eventcnt == epoll_eventmax))
    {
      ev_free (epoll_events);
      epoll_eventmax = array_nextsize (sizeof (struct epoll_event), epoll_eventmax, epoll_eventmax + 1);
      epoll_events = (struct epoll_event *)ev_malloc (sizeof (struct epoll_event) * epoll_eventmax);
    }
}

int inline_size
epoll_init (EV_P_ int flags)
{
  backend_fd = epoll_create (256);

  if (backend_fd < 0)
    return 0;

  fcntl (backend_fd, F_SETFD, FD_CLOEXEC);

  backend_fudge  = 0.; /* kernel sources seem to indicate this to be zero */
  backend_modify = epoll_modify;
  backend_poll   = epoll_poll;

  epoll_eventmax = 64; /* intiial number of events receivable per poll */
  epoll_events = (struct epoll_event *)ev_malloc (sizeof (struct epoll_event) * epoll_eventmax);

  return EVBACKEND_EPOLL;
}

void inline_size
epoll_destroy (EV_P)
{
  ev_free (epoll_events);
}

void inline_size
epoll_fork (EV_P)
{
  close (backend_fd);

  while ((backend_fd = epoll_create (256)) < 0)
    syserr ("(libev) epoll_create");

  fcntl (backend_fd, F_SETFD, FD_CLOEXEC);

  fd_rearm_all (EV_A);
}

