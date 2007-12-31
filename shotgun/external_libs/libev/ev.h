/*
 * libev native API header
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

#ifndef EV_H__
#define EV_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef double ev_tstamp;

/* these priorities are inclusive, higher priorities will be called earlier */
#ifndef EV_MINPRI
# define EV_MINPRI -2
#endif
#ifndef EV_MAXPRI
# define EV_MAXPRI +2
#endif

#ifndef EV_MULTIPLICITY
# define EV_MULTIPLICITY 1
#endif

#ifndef EV_PERIODIC_ENABLE
# define EV_PERIODIC_ENABLE 1
#endif

#ifndef EV_STAT_ENABLE
# define EV_STAT_ENABLE 1
#endif

#ifndef EV_IDLE_ENABLE
# define EV_IDLE_ENABLE 1
#endif

#ifndef EV_FORK_ENABLE
# define EV_FORK_ENABLE 1
#endif

#ifndef EV_EMBED_ENABLE
# define EV_EMBED_ENABLE 1
#endif

/*****************************************************************************/

#if EV_STAT_ENABLE
# include <sys/stat.h>
#endif

/* support multiple event loops? */
#if EV_MULTIPLICITY
struct ev_loop;
# define EV_P struct ev_loop *loop
# define EV_P_ EV_P,
# define EV_A loop
# define EV_A_ EV_A,
# define EV_DEFAULT ev_default_loop (0)
# define EV_DEFAULT_ EV_DEFAULT,
#else
# define EV_P void
# define EV_P_
# define EV_A
# define EV_A_
# define EV_DEFAULT
# define EV_DEFAULT_

# undef EV_EMBED_ENABLE
#endif

/* eventmask, revents, events... */
#define EV_UNDEF            -1L /* guaranteed to be invalid */
#define EV_NONE           0x00L /* no events */
#define EV_READ           0x01L /* ev_io detected read will not block */
#define EV_WRITE          0x02L /* ev_io detected write will not block */
#define EV_IOFDSET        0x80L /* internal use only */
#define EV_TIMEOUT  0x00000100L /* timer timed out */
#define EV_PERIODIC 0x00000200L /* periodic timer timed out */
#define EV_SIGNAL   0x00000400L /* signal was received */
#define EV_CHILD    0x00000800L /* child/pid had status change */
#define EV_STAT     0x00001000L /* stat data changed */
#define EV_IDLE     0x00002000L /* event loop is idling */
#define EV_PREPARE  0x00004000L /* event loop about to poll */
#define EV_CHECK    0x00008000L /* event loop finished poll */
#define EV_EMBED    0x00010000L /* embedded event loop needs sweep */
#define EV_FORK     0x00020000L /* event loop resumed in child */
#define EV_ERROR    0x80000000L /* sent when an error occurs */

/* can be used to add custom fields to all watchers, while losing binary compatibility */
#ifndef EV_COMMON
# define EV_COMMON void *data;
#endif
#ifndef EV_PROTOTYPES
# define EV_PROTOTYPES 1
#endif

#define EV_VERSION_MAJOR 2
#define EV_VERSION_MINOR 0

#ifndef EV_CB_DECLARE
# define EV_CB_DECLARE(type) void (*cb)(EV_P_ struct type *w, int revents);
#endif
#ifndef EV_CB_INVOKE
# define EV_CB_INVOKE(watcher,revents) (watcher)->cb (EV_A_ (watcher), (revents))
#endif

/*
 * struct member types:
 * private: you can look at them, but not change them, and they might not mean anything to you.
 * ro: can be read anytime, but only changed when the watcher isn't active
 * rw: can be read and modified anytime, even when the watcher is active
 */

/* shared by all watchers */
#define EV_WATCHER(type)			\
  int active; /* private */			\
  int pending; /* private */			\
  int priority; /* private */			\
  EV_COMMON /* rw */				\
  EV_CB_DECLARE (type) /* private */

#define EV_WATCHER_LIST(type)			\
  EV_WATCHER (type)				\
  struct ev_watcher_list *next; /* private */

#define EV_WATCHER_TIME(type)			\
  EV_WATCHER (type)				\
  ev_tstamp at;     /* private */

/* base class, nothing to see here unless you subclass */
typedef struct ev_watcher
{
  EV_WATCHER (ev_watcher)
} ev_watcher;

/* base class, nothing to see here unless you subclass */
typedef struct ev_watcher_list
{
  EV_WATCHER_LIST (ev_watcher_list)
} ev_watcher_list;

/* base class, nothing to see here unless you subclass */
typedef struct ev_watcher_time
{
  EV_WATCHER_TIME (ev_watcher_time)
} ev_watcher_time;

/* invoked when fd is either EV_READable or EV_WRITEable */
/* revent EV_READ, EV_WRITE */
typedef struct ev_io
{
  EV_WATCHER_LIST (ev_io)

  int fd;     /* ro */
  int events; /* ro */
} ev_io;

/* invoked after a specific time, repeatable (based on monotonic clock) */
/* revent EV_TIMEOUT */
typedef struct ev_timer
{
  EV_WATCHER_TIME (ev_timer)

  ev_tstamp repeat; /* rw */
} ev_timer;

/* invoked at some specific time, possibly repeating at regular intervals (based on UTC) */
/* revent EV_PERIODIC */
typedef struct ev_periodic
{
  EV_WATCHER_TIME (ev_periodic)

  ev_tstamp offset; /* rw */
  ev_tstamp interval; /* rw */
  ev_tstamp (*reschedule_cb)(struct ev_periodic *w, ev_tstamp now); /* rw */
} ev_periodic;

/* invoked when the given signal has been received */
/* revent EV_SIGNAL */
typedef struct ev_signal
{
  EV_WATCHER_LIST (ev_signal)

  int signum; /* ro */
} ev_signal;

/* invoked when sigchld is received and waitpid indicates the given pid */
/* revent EV_CHILD */
/* does not support priorities */
typedef struct ev_child
{
  EV_WATCHER_LIST (ev_child)

  int pid;     /* ro */
  int rpid;    /* rw, holds the received pid */
  int rstatus; /* rw, holds the exit status, use the macros from sys/wait.h */
} ev_child;

#if EV_STAT_ENABLE
/* st_nlink = 0 means missing file or other error */
# ifdef _WIN32
typedef struct _stati64 ev_statdata;
# else
typedef struct stat ev_statdata;
# endif

/* invoked each time the stat data changes for a given path */
/* revent EV_STAT */
typedef struct ev_stat
{
  EV_WATCHER_LIST (ev_stat)

  ev_timer timer;     /* private */
  ev_tstamp interval; /* ro */
  const char *path;   /* ro */
  ev_statdata prev;   /* ro */
  ev_statdata attr;   /* ro */

  int wd; /* wd for inotify, fd for kqueue */
} ev_stat;
#endif

#if EV_IDLE_ENABLE
/* invoked when the nothing else needs to be done, keeps the process from blocking */
/* revent EV_IDLE */
typedef struct ev_idle
{
  EV_WATCHER (ev_idle)
} ev_idle;
#endif

/* invoked for each run of the mainloop, just before the blocking call */
/* you can still change events in any way you like */
/* revent EV_PREPARE */
typedef struct ev_prepare
{
  EV_WATCHER (ev_prepare)
} ev_prepare;

/* invoked for each run of the mainloop, just after the blocking call */
/* revent EV_CHECK */
typedef struct ev_check
{
  EV_WATCHER (ev_check)
} ev_check;

#if EV_FORK_ENABLE
/* the callback gets invoked before check in the child process when a fork was detected */
typedef struct ev_fork
{
  EV_WATCHER (ev_fork)
} ev_fork;
#endif

#if EV_EMBED_ENABLE
/* used to embed an event loop inside another */
/* the callback gets invoked when the event loop has handled events, and can be 0 */
typedef struct ev_embed
{
  EV_WATCHER (ev_embed)

  struct ev_loop *other; /* ro */
  ev_io io;              /* private */
  ev_prepare prepare;    /* private */
  ev_check check;        /* unused */
  ev_timer timer;        /* unused */
  ev_periodic periodic;  /* unused */
  ev_idle idle;          /* unused */
  ev_fork fork;          /* unused */
} ev_embed;
#endif

/* the presence of this union forces similar struct layout */
union ev_any_watcher
{
  struct ev_watcher w;
  struct ev_watcher_list wl;

  struct ev_io io;
  struct ev_timer timer;
  struct ev_periodic periodic;
  struct ev_signal signal;
  struct ev_child child;
#if EV_STAT_ENABLE
  struct ev_stat stat;
#endif
#if EV_IDLE_ENABLE
  struct ev_idle idle;
#endif
  struct ev_prepare prepare;
  struct ev_check check;
#if EV_FORK_ENABLE
  struct ev_fork fork;
#endif
#if EV_EMBED_ENABLE
  struct ev_embed embed;
#endif
};

/* bits for ev_default_loop and ev_loop_new */
/* the default */
#define EVFLAG_AUTO       0x00000000UL /* not quite a mask */
/* flag bits */
#define EVFLAG_NOENV      0x01000000UL /* do NOT consult environment */
#define EVFLAG_FORKCHECK  0x02000000UL /* check for a fork in each iteration */
/* method bits to be ored together */
#define EVBACKEND_SELECT  0x00000001UL /* about anywhere */
#define EVBACKEND_POLL    0x00000002UL /* !win */
#define EVBACKEND_EPOLL   0x00000004UL /* linux */
#define EVBACKEND_KQUEUE  0x00000008UL /* bsd */
#define EVBACKEND_DEVPOLL 0x00000010UL /* solaris 8 */ /* NYI */
#define EVBACKEND_PORT    0x00000020UL /* solaris 10 */

#if EV_PROTOTYPES
int ev_version_major (void);
int ev_version_minor (void);

unsigned int ev_supported_backends (void);
unsigned int ev_recommended_backends (void);
unsigned int ev_embeddable_backends (void);

ev_tstamp ev_time (void);
void ev_sleep (ev_tstamp delay); /* sleep for a while */

/* Sets the allocation function to use, works like realloc.
 * It is used to allocate and free memory.
 * If it returns zero when memory needs to be allocated, the library might abort
 * or take some potentially destructive action.
 * The default is your system realloc function.
 */
void ev_set_allocator (void *(*cb)(void *ptr, long size));

/* set the callback function to call on a
 * retryable syscall error
 * (such as failed select, poll, epoll_wait)
 */
void ev_set_syserr_cb (void (*cb)(const char *msg));

# if EV_MULTIPLICITY
/* the default loop is the only one that handles signals and child watchers */
/* you can call this as often as you like */
static struct ev_loop *
ev_default_loop (unsigned int flags)
{
  extern struct ev_loop *ev_default_loop_ptr;
  extern struct ev_loop *ev_default_loop_init (unsigned int flags);

  if (!ev_default_loop_ptr)
    ev_default_loop_init (flags);

  return ev_default_loop_ptr;
}

/* create and destroy alternative loops that don't handle signals */
struct ev_loop *ev_loop_new (unsigned int flags);
void ev_loop_destroy (EV_P);
void ev_loop_fork (EV_P);

ev_tstamp ev_now (EV_P); /* time w.r.t. timers and the eventloop, updated after each poll */

# else

int ev_default_loop (unsigned int flags); /* returns true when successful */

static ev_tstamp
ev_now (void)
{
  extern ev_tstamp ev_rt_now;

  return ev_rt_now;
}
# endif

void ev_default_destroy (void); /* destroy the default loop */
/* this needs to be called after fork, to duplicate the default loop */
/* if you create alternative loops you have to call ev_loop_fork on them */
/* you can call it in either the parent or the child */
/* you can actually call it at any time, anywhere :) */
void ev_default_fork (void);

unsigned int ev_backend (EV_P);    /* backend in use by loop */
unsigned int ev_loop_count (EV_P); /* number of loop iterations */
#endif

#define EVLOOP_NONBLOCK	1 /* do not block/wait */
#define EVLOOP_ONESHOT	2 /* block *once* only */
#define EVUNLOOP_CANCEL 0 /* undo unloop */
#define EVUNLOOP_ONE    1 /* unloop once */
#define EVUNLOOP_ALL    2 /* unloop all loops */

#if EV_PROTOTYPES
void ev_loop (EV_P_ int flags);
void ev_unloop (EV_P_ int how); /* set to 1 to break out of event loop, set to 2 to break out of all event loops */

void ev_set_io_collect_interval (EV_P_ ev_tstamp interval); /* sleep at least this time, default 0 */
void ev_set_timeout_collect_interval (EV_P_ ev_tstamp interval); /* sleep at least this time, default 0 */

/*
 * ref/unref can be used to add or remove a refcount on the mainloop. every watcher
 * keeps one reference. if you have a long-runing watcher you never unregister that
 * should not keep ev_loop from running, unref() after starting, and ref() before stopping.
 */
void ev_ref   (EV_P);
void ev_unref (EV_P);

/* convinience function, wait for a single event, without registering an event watcher */
/* if timeout is < 0, do wait indefinitely */
void ev_once (EV_P_ int fd, int events, ev_tstamp timeout, void (*cb)(int revents, void *arg), void *arg);
#endif

/* these may evaluate ev multiple times, and the other arguments at most once */
/* either use ev_init + ev_TYPE_set, or the ev_TYPE_init macro, below, to first initialise a watcher */
#define ev_init(ev,cb_) do {			\
  ((ev_watcher *)(void *)(ev))->active   =	\
  ((ev_watcher *)(void *)(ev))->pending  =	\
  ((ev_watcher *)(void *)(ev))->priority = 0;	\
  ev_set_cb ((ev), cb_);			\
} while (0)

#define ev_io_set(ev,fd_,events_)           do { (ev)->fd = (fd_); (ev)->events = (events_) | EV_IOFDSET; } while (0)
#define ev_timer_set(ev,after_,repeat_)     do { (ev)->at = (after_); (ev)->repeat = (repeat_); } while (0)
#define ev_periodic_set(ev,ofs_,ival_,res_) do { (ev)->offset = (ofs_); (ev)->interval = (ival_); (ev)->reschedule_cb= (res_); } while (0)
#define ev_signal_set(ev,signum_)           do { (ev)->signum = (signum_); } while (0)
#define ev_child_set(ev,pid_)               do { (ev)->pid = (pid_); } while (0)
#define ev_stat_set(ev,path_,interval_)     do { (ev)->path = (path_); (ev)->interval = (interval_); (ev)->wd = -2; } while (0)
#define ev_idle_set(ev)                     /* nop, yes, this is a serious in-joke */
#define ev_prepare_set(ev)                  /* nop, yes, this is a serious in-joke */
#define ev_check_set(ev)                    /* nop, yes, this is a serious in-joke */
#define ev_embed_set(ev,other_)             do { (ev)->other = (other_); } while (0)
#define ev_fork_set(ev)                     /* nop, yes, this is a serious in-joke */

#define ev_io_init(ev,cb,fd,events)         do { ev_init ((ev), (cb)); ev_io_set ((ev),(fd),(events)); } while (0)
#define ev_timer_init(ev,cb,after,repeat)   do { ev_init ((ev), (cb)); ev_timer_set ((ev),(after),(repeat)); } while (0)
#define ev_periodic_init(ev,cb,at,ival,res) do { ev_init ((ev), (cb)); ev_periodic_set ((ev),(at),(ival),(res)); } while (0)
#define ev_signal_init(ev,cb,signum)        do { ev_init ((ev), (cb)); ev_signal_set ((ev), (signum)); } while (0)
#define ev_child_init(ev,cb,pid)            do { ev_init ((ev), (cb)); ev_child_set ((ev),(pid)); } while (0)
#define ev_stat_init(ev,cb,path,interval)   do { ev_init ((ev), (cb)); ev_stat_set ((ev),(path),(interval)); } while (0)
#define ev_idle_init(ev,cb)                 do { ev_init ((ev), (cb)); ev_idle_set ((ev)); } while (0)
#define ev_prepare_init(ev,cb)              do { ev_init ((ev), (cb)); ev_prepare_set ((ev)); } while (0)
#define ev_check_init(ev,cb)                do { ev_init ((ev), (cb)); ev_check_set ((ev)); } while (0)
#define ev_embed_init(ev,cb,other)          do { ev_init ((ev), (cb)); ev_embed_set ((ev),(other)); } while (0)
#define ev_fork_init(ev,cb)                 do { ev_init ((ev), (cb)); ev_fork_set ((ev)); } while (0)

#define ev_is_pending(ev)                   (0 + ((ev_watcher *)(void *)(ev))->pending) /* ro, true when watcher is waiting for callback invocation */
#define ev_is_active(ev)                    (0 + ((ev_watcher *)(void *)(ev))->active) /* ro, true when the watcher has been started */

#define ev_priority(ev)                     ((((ev_watcher *)(void *)(ev))->priority) + 0)
#define ev_cb(ev)                           (ev)->cb /* rw */
#define ev_set_priority(ev,pri)             ((ev_watcher *)(void *)(ev))->priority = (pri)

#ifndef ev_set_cb
# define ev_set_cb(ev,cb_)                  ev_cb (ev) = (cb_)
#endif

/* stopping (enabling, adding) a watcher does nothing if it is already running */
/* stopping (disabling, deleting) a watcher does nothing unless its already running */
#if EV_PROTOTYPES

/* feeds an event into a watcher as if the event actually occured */
/* accepts any ev_watcher type */
void ev_feed_event     (EV_P_ void *w, int revents);
void ev_feed_fd_event  (EV_P_ int fd, int revents);
void ev_feed_signal_event (EV_P_ int signum);
void ev_invoke         (EV_P_ void *w, int revents);
int  ev_clear_pending  (EV_P_ void *w);

void ev_io_start       (EV_P_ ev_io *w);
void ev_io_stop        (EV_P_ ev_io *w);

void ev_timer_start    (EV_P_ ev_timer *w);
void ev_timer_stop     (EV_P_ ev_timer *w);
/* stops if active and no repeat, restarts if active and repeating, starts if inactive and repeating */
void ev_timer_again    (EV_P_ ev_timer *w);

#if EV_PERIODIC_ENABLE
void ev_periodic_start (EV_P_ ev_periodic *w);
void ev_periodic_stop  (EV_P_ ev_periodic *w);
void ev_periodic_again (EV_P_ ev_periodic *w);
#endif

/* only supported in the default loop */
void ev_signal_start   (EV_P_ ev_signal *w);
void ev_signal_stop    (EV_P_ ev_signal *w);

/* only supported in the default loop */
void ev_child_start    (EV_P_ ev_child *w);
void ev_child_stop     (EV_P_ ev_child *w);

# if EV_STAT_ENABLE
void ev_stat_start     (EV_P_ ev_stat *w);
void ev_stat_stop      (EV_P_ ev_stat *w);
void ev_stat_stat      (EV_P_ ev_stat *w);
# endif

# if EV_IDLE_ENABLE
void ev_idle_start     (EV_P_ ev_idle *w);
void ev_idle_stop      (EV_P_ ev_idle *w);
# endif

void ev_prepare_start  (EV_P_ ev_prepare *w);
void ev_prepare_stop   (EV_P_ ev_prepare *w);

void ev_check_start    (EV_P_ ev_check *w);
void ev_check_stop     (EV_P_ ev_check *w);

# if EV_FORK_ENABLE
void ev_fork_start     (EV_P_ ev_fork *w);
void ev_fork_stop      (EV_P_ ev_fork *w);
# endif

# if EV_EMBED_ENABLE
/* only supported when loop to be embedded is in fact embeddable */
void ev_embed_start    (EV_P_ ev_embed *w);
void ev_embed_stop     (EV_P_ ev_embed *w);
void ev_embed_sweep    (EV_P_ ev_embed *w);
# endif

#endif

#ifdef __cplusplus
}
#endif

#endif

