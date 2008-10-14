/* A C++ wrapper around libev, which Rubinius uses for all event handling.
 * This is the folcrum of the thread subsystem. */

#include <sstream>
#include <vector>

#include "prelude.hpp"
#include "object.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include "builtin/tuple.hpp"
#include "builtin/integer.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/io.hpp"
#include "builtin/thread.hpp"

#include "event.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ev.h>

namespace rubinius {

  namespace event {

    static void dispatch(Event *obj) {
      if(obj->activated()) {
        if(obj->loop) {
          obj->loop->remove_event(obj);
        }

        if(obj->tracked()) {
          delete obj;
        } else {
          obj->stop();
        }
      }
    }

    template <typename S>
      void tramp(EV_P_ S* ev, int revents) {
        event::dispatch((Event*)ev->data);
      }

    Event::Event(STATE, ObjectCallback* chan) :
      state(state), channel(chan), id(0),
      buffer(state), loop(NULL) { }

    void IO::stop() {
      ev_io_stop(loop->base, &ev);
    }

    void IO::start() {
      ev_io_start(loop->base, &ev);
    }

    bool IO::for_fd_p(int in) {
      if(fd == in) return true;
      return false;
    }

    Write::Write(STATE, ObjectCallback* chan, int fd) : IO(state, chan) {
      ev_io_init(&ev, event::tramp<struct ev_io>, fd, EV_WRITE);
      ev.data = this;
    }

    bool Write::activated() {
      channel->call(Qnil);
      return true;
    }

    Read::Read(STATE, ObjectCallback* chan, int ifd) :
        IO(state, chan), count(0) {
      fd = ifd;
      ev_io_init(&ev, event::tramp<struct ev_io>, fd, EV_READ);
      ev.data = this;
    }

    void Read::into_buffer(rubinius::IOBuffer *buf, size_t bytes) {
      count = bytes;
      buffer.set(buf);
    }

    bool Read::activated() {
      OBJECT ret;

      if(buffer->nil_p()) {
        ret = Integer::from(state, fd);
      } else {
        /* the + 1 is for the null on the end */
        size_t bytes_to_read = count + 1;

        if(buffer->left() < bytes_to_read) {
          bytes_to_read = buffer->left() - 1;
        }

        char* start = buffer->at_unused();
        while(1) {
          int i = read(fd, start, bytes_to_read);

          /* EOF seen */
          if(i == 0) {
            ret = Qnil;

          /* didn't work out... */
          } else if(i == -1) {
            /* we were interrupted, how rude. go again. */
            if(errno == EINTR) continue;

            /* not sure. Send a system error */
            ret = Tuple::from(state, 2, state->symbol("error"), Fixnum::from(errno));
          } else {
            /* clamp */
            start[i] = 0;

            buffer->read_bytes(state, i);
            ret = Fixnum::from(i);
          }

          break;
        }
      }

      channel->call(ret);

      return true;
    }

    Signal::Signal(STATE, ObjectCallback *chan, int sig):
        Event(state, chan), signal(sig) {
      ev_signal_init(&ev, event::tramp<struct ev_signal>, sig);
      ev.data = this;
    }

    void Signal::start() {
      // Only one signal event is valid per loop. We remove any that
      // already exist.
      loop->remove_signal(signal);
      ev_signal_start(loop->base, &ev);
    }

    void Signal::stop() {
      ev_signal_stop(loop->base, &ev);
    }

    bool Signal::activated() {
      channel->call(state->current_thread());
      return false;
    }

    Timer::Timer(STATE, ObjectCallback* chan, double seconds, OBJECT obj) :
        Event(state, chan), tag(obj) {
      ev_timer_init(&ev, event::tramp<struct ev_timer>, (ev_tstamp)seconds, 0);
      ev.data = this;
    }

    void Timer::start() {
      ev_timer_start(loop->base, &ev);
    }

    void Timer::stop() {
      ev_timer_stop(loop->base, &ev);
    }

    bool Timer::activated() {
      channel->call(tag);
      return true;
    }


/* SIGCHLD */


    Child::Child(ObjectCallback* channel, pid_t pid, int opts):
      channel_(channel), options_(opts), pid_(pid) {
    }

    Child::~Child() {}

    void Child::add(STATE, ObjectCallback* channel, pid_t pid, int opts) {
      Child::waiters().push_back(new Child(channel, pid, opts));

      /*  This seems a bit cheap, but we need to force a check to
       *  catch the case where wait is called before any child is
       *  created, because such an occurrence must result in ECHILD.
       */
      if(Child::waiters().size() == 1) {
        Child::find_finished(state);
      }
    }

    /**
     *  The idea here is that this method is called each time we receive
     *  a SIGCHLD. Since we do not know which child exited, we check each
     *  using waitpid(). The WNOHANG means that if the child is still up,
     *  we can just move on to the next candidate and come back to check
     *  later.
     *
     *  To avoid a problem with calling wait before any children exist
     *  not raising an error, ::add() explicitly runs this method when
     *  the first waiter is added (whether at the start or at some point
     *  when all waiters have been removed previously.)
     *
     *  TODO: Support WUNTRACED?
     */
    void Child::find_finished(VM* state) {
      Waiters& all = Child::waiters();

      for (Waiters::iterator it = all.begin(); it != all.end(); /* Updated in body */) {
        int status = 0;
        Child* waiter = *it;

        pid_t pid = ::waitpid(waiter->pid(), &status, WNOHANG);

        switch(pid) {
        case -1:
          if(errno == EINTR) {
            continue; /* With the same iterator */
          }

          if(errno == ECHILD) {
            waiter->channel()->call(Qfalse);
          }

          break;

        case 0:       /* No stopped children. */
          if(waiter->options() & WNOHANG) {
            waiter->channel()->call(Qnil);
            it = all.erase(it);
            continue;
          }

          /* Hangers continue hanging */
          break;

        default:      /* Found it */
          Object* SP = Qtrue;

          if ( WIFEXITED(status) ) {
            SP = as<Object>(Fixnum::from(WEXITSTATUS(status)));
          }

          waiter->channel()->call(Tuple::from(state, 2, Fixnum::from(pid), SP));
          it = all.erase(it);
          continue;
        }

        ++it;
      }
    }

    Loop::Loop(struct ev_loop *loop) :
      base(loop), event_ids(0), owner(false) { }

    Loop::Loop(int opts) : event_ids(0), owner(false) {
      base = ev_default_loop(opts);

      /* TODO: Should fail here if default returns NULL */
    }

    /* Gives this loop ownership of +ev+, letting it delete +ev+
     * when +ev+ is done, then starts the event. */
    void Loop::start(Event* ev) {
      ev->loop = this;
      ev->id = ++event_ids;
      ev->start();

      // It's important this is last. Signal::start removes older Signal
      // events by looking through +events+ on start. We don't want it
      // to remove itself, so we do this after the event has actually
      // started.
      events.push_back(ev);
    }

    Loop::~Loop() {
      if (owner) {
        ev_loop_destroy(base);
      }
    }

    size_t Loop::num_of_events() {
      return events.size();
    }

    size_t Loop::loop_count() {
      return (size_t)ev_loop_count(base);
    }

    void Loop::poll() {
      ev_loop(base, EVLOOP_NONBLOCK);
    }

    void Loop::run_and_wait() {
      ev_loop(base, EVLOOP_ONESHOT);
    }

    void Loop::clear_by_fd(int fd) {
      std::vector<Event*>::iterator it;
      for(it = events.begin(); it != events.end();) {
        if((*it)->for_fd_p(fd)) {
          delete *it;
          it = events.erase(it);
        } else {
          it++;
        }
      }
    }

    void Loop::clear_by_channel(void* chan) {
      std::vector<Event*>::iterator it;
      for(it = events.begin(); it != events.end();) {
        if((*it)->channel == chan) {
          delete *it;
          it = events.erase(it);
        } else {
          it++;
        }
      }
    }

    void Loop::clear_by_id(size_t id) {
      std::vector<Event*>::iterator it;
      for(it = events.begin(); it != events.end();) {
        if((*it)->id == id) {
          delete *it;
          it = events.erase(it);
        } else {
          it++;
        }
      }
    }

    void Loop::remove_event(Event *ev) {
      std::vector<Event*>::iterator it;
      for(it = events.begin(); it != events.end();) {
        if(*it == ev) {
          it = events.erase(it);
        } else {
          it++;
        }
      }
    }

    // Look through events for a Signal object for +signal_number+
    // If we find one, send nil to the channel and remove it.
    void Loop::remove_signal(int signal_number) {
      for(std::vector<Event*>::iterator it = events.begin();
          it != events.end();) {

        Event* ev = *it;
        if(Signal* sig = dynamic_cast<Signal*>(ev)) {
          if(sig->signal == signal_number) {
            sig->channel->call(Qnil);
            sig->stop();
            it = events.erase(it);
            continue;
          }
        }

        it++;
      }
    }

  } // event
} // rubinius
