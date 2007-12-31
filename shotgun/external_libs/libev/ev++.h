/*
 * libev simple C++ wrapper classes
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

#ifndef EVPP_H__
#define EVPP_H__

#ifdef EV_H
# include EV_H
#else
# include <ev.h>
#endif

namespace ev {

  template<class ev_watcher, class watcher>
  struct base : ev_watcher
  {
    #if EV_MULTIPLICITY
      EV_P;

      void set (EV_P)
      {
        this->EV_A = EV_A;
      }
    #endif

    base ()
    {
      ev_init (this, 0);
    }

    void set_ (void *data, void (*cb)(EV_P_ ev_watcher *w, int revents))
    {
      this->data = data;
      ev_set_cb (static_cast<ev_watcher *>(this), cb);
    }

    // method callback
    template<class K, void (K::*method)(watcher &w, int)>
    void set (K *object)
    {
      set_ (object, method_thunk<K, method>);
    }

    template<class K, void (K::*method)(watcher &w, int)>
    static void method_thunk (EV_P_ ev_watcher *w, int revents)
    {
      K *obj = static_cast<K *>(w->data);
      (obj->*method) (*static_cast<watcher *>(w), revents);
    }

    // const method callback
    template<class K, void (K::*method)(watcher &w, int) const>
    void set (const K *object)
    {
      set_ (object, const_method_thunk<K, method>);
    }

    template<class K, void (K::*method)(watcher &w, int) const>
    static void const_method_thunk (EV_P_ ev_watcher *w, int revents)
    {
      K *obj = static_cast<K *>(w->data);
      (static_cast<K *>(w->data)->*method) (*static_cast<watcher *>(w), revents);
    }

    // function callback
    template<void (*function)(watcher &w, int)>
    void set (void *data = 0)
    {
      set_ (data, function_thunk<function>);
    }

    template<void (*function)(watcher &w, int)>
    static void function_thunk (EV_P_ ev_watcher *w, int revents)
    {
      function (*static_cast<watcher *>(w), revents);
    }

    // simple callback
    template<class K, void (K::*method)()>
    void set (K *object)
    {
      set_ (object, method_noargs_thunk<K, method>);
    }

    template<class K, void (K::*method)()>
    static void method_noargs_thunk (EV_P_ ev_watcher *w, int revents)
    {
      K *obj = static_cast<K *>(w->data);
      (obj->*method) ();
    }

    void operator ()(int events = EV_UNDEF)
    {
      return ev_cb (static_cast<ev_watcher *>(this))
        (static_cast<ev_watcher *>(this), events);
    }

    bool is_active () const
    {
      return ev_is_active (static_cast<const ev_watcher *>(this));
    }

    bool is_pending () const
    {
      return ev_is_pending (static_cast<const ev_watcher *>(this));
    }
  };

  enum {
    UNDEF    = EV_UNDEF,
    NONE     = EV_NONE,
    READ     = EV_READ,
    WRITE    = EV_WRITE,
    TIMEOUT  = EV_TIMEOUT,
    PERIODIC = EV_PERIODIC,
    SIGNAL   = EV_SIGNAL,
    CHILD    = EV_CHILD,
    STAT     = EV_STAT,
    IDLE     = EV_IDLE,
    CHECK    = EV_CHECK,
    PREPARE  = EV_PREPARE,
    FORK     = EV_FORK,
    EMBED    = EV_EMBED,
    ERROR    = EV_ERROR,
  };

  typedef ev_tstamp tstamp;

  inline ev_tstamp now (EV_P)
  {
    return ev_now (EV_A);
  }

  #if EV_MULTIPLICITY
    #define EV_CONSTRUCT                                                                \
      (EV_P = EV_DEFAULT)                                                               \
      {                                                                                 \
        set (EV_A);                                                                     \
      }
  #else
    #define EV_CONSTRUCT                                                                \
      ()                                                                                \
      {                                                                                 \
      }
  #endif

  /* using a template here would require quite a bit more lines,
   * so a macro solution was chosen */
  #define EV_BEGIN_WATCHER(cppstem,cstem)	                                        \
                                                                                        \
  struct cppstem : base<ev_ ## cstem, cppstem>                                          \
  {                                                                                     \
    void start ()                                                                       \
    {                                                                                   \
      ev_ ## cstem ## _start (EV_A_ static_cast<ev_ ## cstem *>(this));                 \
    }                                                                                   \
                                                                                        \
    void stop ()                                                                        \
    {                                                                                   \
      ev_ ## cstem ## _stop (EV_A_ static_cast<ev_ ## cstem *>(this));                  \
    }                                                                                   \
                                                                                        \
    cppstem EV_CONSTRUCT                                                                \
                                                                                        \
    ~cppstem ()                                                                         \
    {                                                                                   \
      stop ();                                                                          \
    }                                                                                   \
                                                                                        \
    using base<ev_ ## cstem, cppstem>::set;                                             \
                                                                                        \
  private:                                                                              \
                                                                                        \
    cppstem (const cppstem &o)								\
    { /* disabled */ }                                        				\
                                                                                        \
    void operator =(const cppstem &o) { /* disabled */ }                                \
                                                                                        \
  public:

  #define EV_END_WATCHER(cppstem,cstem)	                                                \
  };

  EV_BEGIN_WATCHER (io, io)
    void set (int fd, int events)
    {
      int active = is_active ();
      if (active) stop ();
      ev_io_set (static_cast<ev_io *>(this), fd, events);
      if (active) start ();
    }

    void set (int events)
    {
      int active = is_active ();
      if (active) stop ();
      ev_io_set (static_cast<ev_io *>(this), fd, events);
      if (active) start ();
    }

    void start (int fd, int events)
    {
      set (fd, events);
      start ();
    }
  EV_END_WATCHER (io, io)

  EV_BEGIN_WATCHER (timer, timer)
    void set (ev_tstamp after, ev_tstamp repeat = 0.)
    {
      int active = is_active ();
      if (active) stop ();
      ev_timer_set (static_cast<ev_timer *>(this), after, repeat);
      if (active) start ();
    }

    void start (ev_tstamp after, ev_tstamp repeat = 0.)
    {
      set (after, repeat);
      start ();
    }

    void again ()
    {
      ev_timer_again (EV_A_ static_cast<ev_timer *>(this));
    }
  EV_END_WATCHER (timer, timer)

  #if EV_PERIODIC_ENABLE
  EV_BEGIN_WATCHER (periodic, periodic)
    void set (ev_tstamp at, ev_tstamp interval = 0.)
    {
      int active = is_active ();
      if (active) stop ();
      ev_periodic_set (static_cast<ev_periodic *>(this), at, interval, 0);
      if (active) start ();
    }

    void start (ev_tstamp at, ev_tstamp interval = 0.)
    {
      set (at, interval);
      start ();
    }

    void again ()
    {
      ev_periodic_again (EV_A_ static_cast<ev_periodic *>(this));
    }
  EV_END_WATCHER (periodic, periodic)
  #endif

  EV_BEGIN_WATCHER (sig, signal)
    void set (int signum)
    {
      int active = is_active ();
      if (active) stop ();
      ev_signal_set (static_cast<ev_signal *>(this), signum);
      if (active) start ();
    }

    void start (int signum)
    {
      set (signum);
      start ();
    }
  EV_END_WATCHER (sig, signal)

  EV_BEGIN_WATCHER (child, child)
    void set (int pid)
    {
      int active = is_active ();
      if (active) stop ();
      ev_child_set (static_cast<ev_child *>(this), pid);
      if (active) start ();
    }

    void start (int pid)
    {
      set (pid);
      start ();
    }
  EV_END_WATCHER (child, child)

  #if EV_STAT_ENABLE
  EV_BEGIN_WATCHER (stat, stat)
    void set (const char *path, ev_tstamp interval = 0.)
    {
      int active = is_active ();
      if (active) stop ();
      ev_stat_set (static_cast<ev_stat *>(this), path, interval);
      if (active) start ();
    }

    void start (const char *path, ev_tstamp interval = 0.)
    {
      stop ();
      set (path, interval);
      start ();
    }

    void update ()
    {
      ev_stat_stat (EV_A_ static_cast<ev_stat *>(this));
    }
  EV_END_WATCHER (stat, stat)
  #endif

  EV_BEGIN_WATCHER (idle, idle)
    void set () { }
  EV_END_WATCHER (idle, idle)

  EV_BEGIN_WATCHER (prepare, prepare)
    void set () { }
  EV_END_WATCHER (prepare, prepare)

  EV_BEGIN_WATCHER (check, check)
    void set () { }
  EV_END_WATCHER (check, check)

  #if EV_EMBED_ENABLE
  EV_BEGIN_WATCHER (embed, embed)
    void start (struct ev_loop *embedded_loop)
    {
      stop ();
      ev_embed_set (static_cast<ev_embed *>(this), embedded_loop);
      start ();
    }

    void sweep ()
    {
      ev_embed_sweep (EV_A_ static_cast<ev_embed *>(this));
    }
  EV_END_WATCHER (embed, embed)
  #endif

  #if EV_FORK_ENABLE
  EV_BEGIN_WATCHER (fork, fork)
    void set () { }
  EV_END_WATCHER (fork, fork)
  #endif

  #undef EV_CONSTRUCT
  #undef EV_BEGIN_WATCHER
  #undef EV_END_WATCHER
}

#endif

